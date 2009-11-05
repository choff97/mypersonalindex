#include "queries.h"
#include "globals.h"
#include "functions.h"

//enum { closingPrices_Date, closingPrices_Ticker, closingPrices_Price, closingPrices_Change };
const QStringList queries::closingPricesColumns = QStringList() << "Date" << "Ticker" << "Price" << "Change";

//enum { splits_Date, splits_Ticker, splits_Ratio };
const QStringList queries::splitsColumns = QStringList() << "Date" << "Ticker" << "Ratio";

//enum { dividends_Date, dividends_Ticker, dividends_Amount };
const QStringList queries::dividendsColumns = QStringList() << "Date" << "Ticker" << "Amount";

//enum { statMapping_PortfolioID, statMapping_StatID, statMapping_Sequence };
const QStringList queries::statMappingColumns = QStringList() << "PortfolioID" << "StatID" << "Sequence";

//enum { dividends_Date, dividends_Ticker, dividends_Amount };
//const QStringList queries::tradesColumns = QStringList() << "ID" << "Portfolio" << "TickerID" << "Ticker"
//    << "Date" << "Shares" << "Price" << "Custom";

//enum { tickersAAColumns_TickerID, tickersAAColumns_AAID, tickersAAColumns_Percent };
const QStringList queries::tickersAAColumns = QStringList() << "TickerID" << "AAID" << "Percent";

const QString queries::table_AA = "AA";
const QString queries::table_Acct = "Accounts";
const QString queries::table_AvgPricePerShare = "AvgPricePerShare";
const QString queries::table_ClosingPrices = "ClosingPrices";
const QString queries::table_Dividends = "Dividends";
const QString queries::table_NAV = "NAV";
const QString queries::table_Portfolios = "Portfolios";
const QString queries::table_Settings = "Settings";
const QString queries::table_Splits = "Splits";
const QString queries::table_Stat = "Stat";
const QString queries::table_StatMapping = "StatMapping";
const QString queries::table_Tickers = "Tickers";
const QString queries::table_TickersAA = "TickersAA";
const QString queries::table_TickersTrades = "TickersTrades";
const QString queries::table_Trades = "Trades";

queries::queries(QSqlDatabase database): db(database)
{
    db.open();
    QSqlQuery("SELECT load_extension('libsqlitefunctions.so')", db);

//    QString location = getDatabaseLocation();
//    if (QSqlDatabase::contains(location))
//        db = QSqlDatabase::database(location);
//    else
//    {
//        db = QSqlDatabase::addDatabase("QSQLITE", location);
//        db.setDatabaseName(location);
//
//    }
}

QString queries::getDatabaseLocation()
{
    QSettings cfg(QSettings::IniFormat, QSettings::UserScope, "MyPersonalIndex", "MPI");
    return QFileInfo(cfg.fileName()).absolutePath().append("/MPI.sqlite");
}

void queries::executeNonQuery(queryInfo *q)
{
    if (!q)
        return;

    QSqlQuery query(db);
    query.prepare(q->sql);
    foreach(const parameter &p, q->parameters)
        query.bindValue(p.name, p.value);

    query.exec();
    delete q;
}

//QSqlQueryModel* queries::executeDataSet(queryInfo *q)
//{
//    if (!q)
//        return 0;
//
//    QSqlQuery query(db);
//    query.prepare(q->sql);
//    foreach(const parameter &p, q->parameters)
//        query.bindValue(p.name, p.value);
//
//    query.exec();
//
//    if(!query.isActive())
//    {
//        delete q;
//        return 0;
//    }
//
//    QSqlQueryModel *dataset = new QSqlQueryModel();
//    dataset->setQuery(query);
//
//    delete q;
//    return dataset;
//}

void queries::executeTableUpdate(const QString &tableName, const QMap<QString /* column name */, QVariantList /* values to be inserted */> &values)
{
    if (tableName.isEmpty() || values.isEmpty())
        return;

    db.transaction();

    QSqlQuery query(db);
    QStringList parameters, columns;
    QList<QVariantList> binds = values.values();
    QString sql("INSERT INTO %1(%2) VALUES (%3)");

    foreach(const QString &column, values.keys())
    {
        parameters.append("?");
        columns.append(column);
    }

    query.prepare(sql.arg(tableName, columns.join(","), parameters.join(",")));

    int count = binds.at(0).count();
    for (int i = 1; i < binds.count(); ++i)
        if (binds.at(i).count() != count) // all the lists must be the same size
        {
            db.commit();
            return;
        }

    for (int i = 0; i < count; ++i)
    {
        for (int x = 0; x < binds.count(); ++x)
            query.addBindValue(binds.at(x).at(i));
        query.exec();
        QString s= query.lastError().text();
        s.append(" ");
    }

    db.commit();
}

QSqlQuery* queries::executeResultSet(queryInfo *q)
{
    if (!q)
        return 0;

    QSqlQuery *query = new QSqlQuery(db);
    query->setForwardOnly(true);
    query->prepare(q->sql);
    foreach(const parameter &p, q->parameters)
        query->bindValue(p.name, p.value);

    query->exec();
    delete q;

    if (query->isActive() && query->first())
        return query;

    return 0;
}

QVariant queries::executeScalar(queryInfo *q, const QVariant &nullValue)
{
    if (!q)
        return QVariant();

    QSqlQuery query(db);
    query.setForwardOnly(true);
    query.prepare(q->sql);
    foreach(const parameter &p, q->parameters)
        query.bindValue(p.name, p.value);

    query.exec();

    if (query.isActive() && query.first())
    {
        QVariant retValue = query.value(0);
        delete q;
        return retValue;
    }

    delete q;
    return nullValue;
}

queries::queryInfo* queries::deleteItem(const QString &table, const int &id)
{
    return new queryInfo(
        QString("DELETE FROM %1 WHERE ID = :ID").arg(table),
        QList<parameter>()
            << parameter(":ID", id)
    );
}

queries::queryInfo* queries::deletePortfolioItems(const QString &table, const int &portfolioID)
{
    return new queryInfo(
        QString("DELETE FROM %1 WHERE PortfolioID = :ID").arg(table),
        QList<parameter>()
            << parameter(":ID", portfolioID)
    );
}

queries::queryInfo* queries::deletePortfolioItems(const QString &table, const int &portfolioID, const int &startingDate)
{
    return new queryInfo(
        QString("DELETE FROM %1 WHERE PortfolioID = :ID AND Date >= :Date").arg(table),
        QList<parameter>()
            << parameter(":ID", portfolioID)
            << parameter(":Date", startingDate)
    );
}

queries::queryInfo* queries::deleteTickerItems(const QString &table, const int &tickerID)
{
    return new queryInfo(
        QString("DELETE FROM %1 WHERE TickerID = :ID").arg(table),
        QList<parameter>()
            << parameter(":ID", tickerID)
    );
}

queries::queryInfo* queries::getIdentity()
{
    return new queryInfo(
        "SELECT last_insert_rowid()",
        QList<parameter>()
    );
}

queries::queryInfo* queries::getVersion()
{
    return new queryInfo(
        "SELECT Version FROM Settings",
        QList<parameter>()
    );
}

queries::queryInfo* queries::getDates()
{
    return new queryInfo(
        "SELECT DISTINCT Date FROM ClosingPrices ORDER BY Date",
        QList<parameter>()
    );
}

queries::queryInfo* queries::updateSettings(const globals::settings &s)
{
    QString sql = "UPDATE Settings SET Splits = :Splits, DataStartDate = :DataStartDate, TickersIncludeDividends = :TickersIncludeDividends,"
            " LastPortfolio = :LastPortfolio, Version = :Version, WindowState = :WindowState";
    QList<parameter> params;
    params  << parameter(":Splits", (int)s.splits)
            << parameter(":DataStartDate", s.dataStartDate)
            << parameter(":TickersIncludeDividends", (int)s.tickersIncludeDividends)
            << parameter(":LastPortfolio", s.lastPortfolio)
            << parameter(":WindowState", (int)s.state)
            << parameter(":Version", s.version);

    if (s.state == Qt::WindowNoState) // only save size and position if the window is in normal state
    {
        sql.append(", WindowX = :WindowX, WindowY = :WindowY, WindowHeight = :WindowHeight, WindowWidth = :WindowWidth");
        params  << parameter(":WindowX", s.windowLocation.x())
                << parameter(":WindowY", s.windowLocation.y())
                << parameter(":WindowHeight", s.windowSize.height())
                << parameter(":WindowWidth", s.windowSize.width());
    }

    return new queryInfo(sql, params);
}

queries::queryInfo* queries::getSettings()
{
    return new queryInfo(
        "SELECT DataStartDate, LastPortfolio, WindowX, WindowY, WindowHeight, WindowWidth, WindowState,"
            " Splits, TickersIncludeDividends, Version FROM Settings",
        QList<parameter>()
    );
}

queries::queryInfo* queries::getAA()
{
    return new queryInfo(
        "SELECT ID, PortfolioID, Description, Target FROM AA ORDER BY PortfolioID",
        QList<parameter>()
    );
}

queries::queryInfo* queries::updateAA(const int &portfolioID, const globals::assetAllocation &aa)
{
    if(aa.id == -1) // insert new
    {
        return new queryInfo(
            "INSERT INTO AA (PortfolioID, Description, Target)"
            " VALUES (:PortfolioID, :Description, :Target)",
            QList<parameter>()
                << parameter(":PortfolioID", portfolioID)
                << parameter(":Description", aa.description)
                << parameter(":Target", functions::doubleToNull(aa.target))
        );
    }
    else // update
    {
        return new queryInfo(
            "UPDATE AA SET Description = :Description, Target = :Target WHERE ID = :AAID",
            QList<parameter>()
                << parameter(":Description", aa.description)
                << parameter(":Target", functions::doubleToNull(aa.target))
                << parameter(":AAID", aa.id)
        );
    }
}

queries::queryInfo* queries::getAcct()
{
    return new queryInfo(
        "SELECT ID, PortfolioID, Description, TaxRate, TaxDeferred FROM Acct ORDER BY PortfolioID",
        QList<parameter>()
    );
}

queries::queryInfo* queries::updateAcct(const int &portfolioID, const globals::account &acct)
{
    if(acct.id == -1) // insert new
    {
        return new queryInfo(
            "INSERT INTO Acct (PortfolioID, Description, TaxRate, TaxDeferred)"
            " VALUES (:PortfolioID, :Description, :TaxRate, :TaxDeferred)",
            QList<parameter>()
                << parameter(":PortfolioID", portfolioID)
                << parameter(":Description", acct.description)
                << parameter(":TaxRate", functions::doubleToNull(acct.taxRate))
                << parameter(":TaxDeferred", (int)acct.taxDeferred)
        );
    }
    else // update
    {
        return new queryInfo(
            "UPDATE Acct SET Description = :Description, TaxRate = :TaxRate, TaxDeferred = :TaxDeferred WHERE ID = :AcctID",
            QList<parameter>()
                << parameter(":Description", acct.description)
                << parameter(":TaxRate", functions::doubleToNull(acct.taxRate))
                << parameter(":TaxDeferred", acct.taxDeferred)
                << parameter(":AcctID", acct.id)
        );
    }
}

queries::queryInfo* queries::updatePortfolio(const globals::portfolio& p)
{
    QList<parameter> params;
    params
        << parameter(":Description", p.description)
        << parameter(":StartValue", p.startValue)
        << parameter(":AAThreshold", p.aaThreshold)
        << parameter(":ThresholdMethod", (int)p.aaThresholdMethod)
        << parameter(":CostCalc", (int)p.costCalc)
        << parameter(":StartDate", p.origStartDate)
        << parameter(":Dividends", (int)p.dividends)
        << parameter(":HoldingsShowHidden", (int)p.holdingsShowHidden)
        << parameter(":HoldingsSort", p.holdingsSort)
        << parameter(":AAShowBlank", (int)p.aaShowBlank)
        << parameter(":AASort", p.aaSort)
        << parameter(":CorrelationShowHidden", (int)p.correlationShowHidden)
        << parameter(":AcctShowBlank", (int)p.acctShowBlank)
        << parameter(":AcctSort", p.acctSort)
        << parameter(":NAVSortDesc", (int)p.navSortDesc);

    if(p.id == -1) // insert new
    {
        return new queryInfo(
            "INSERT INTO Portfolios (Description, StartValue, AAThreshold, ThresholdMethod, CostCalc, StartDate, Dividends, HoldingsShowHidden,"
                " HoldingsSort, AAShowBlank, AASort, CorrelationShowHidden, AcctShowBlank, AcctSort, NAVSortDesc)"
            " VALUES (:Description, :StartValue, :AAThreshold, :ThresholdMethod, :CostCalc, :StartDate, :Dividends, :HoldingsShowHidden,"
                " :HoldingsSort, :AAShowBlank, :AASort, :CorrelationShowHidden, :AcctShowBlank, :AcctSort, :NAVSortDesc)",
            params
        );
    }
    else // update
    {
        params << parameter(":PortfolioID", p.id);
        return new queryInfo(
            "UPDATE Portfolios SET Description = :Description, Dividends = :Dividends, StartValue = :StartValue, CostCalc = :CostCalc, AAThreshold = :AAThreshold,"
                " ThresholdMethod = :ThresholdMethod, StartDate = :StartDate, HoldingsShowHidden = :HoldingsShowHidden, NAVSortDesc = :NAVSortDesc, AAShowBlank = :ShowAABlank,"
                " HoldingsSort = :HoldingsSort, AASort = :AASort, CorrelationShowHidden = :CorrelationShowHidden, AcctShowBlank = :ShowAcctBlank, AcctSort = :AcctSort"
                " WHERE ID = :PortfolioID",
            params
        );
    }
}

queries::queryInfo* queries::getPortfolio()
{
    return new queryInfo(
        "SELECT ID, Description, Dividends, StartValue, CostCalc, AAThreshold, ThresholdMethod,"
            " StartDate, HoldingsShowHidden, HoldingsSort, NAVSortDesc, AASort, AAShowBlank,"
            " CorrelationShowHidden, AcctSort, AcctShowBlank "
            " FROM Portfolios",
        QList<parameter>()
    );
}

queries::queryInfo* queries::updateStat(const globals::statistic &stat)
{
    if(stat.id == -1) // insert new
    {
        return new queryInfo(
            "INSERT INTO Stat (Description, SQL, Format)"
            " VALUES (:Description, :SQL, :Format)",
            QList<parameter>()
                << parameter(":Description", stat.description)
                << parameter(":SQL", stat.sql)
                << parameter(":Format", (int)stat.format)
        );
    }
    else // update
    {
        return new queryInfo(
            "UPDATE Stat SET Description = :Description, SQL = :SQL, Format = :Format WHERE ID = :StatID",
            QList<parameter>()
                << parameter(":Description", stat.description)
                << parameter(":SQL", stat.sql)
                << parameter(":Format", stat.format)
                << parameter(":StatID", stat.id)
        );
    }
}

queries::queryInfo* queries::getStat()
{
    return new queryInfo(
        "SELECT ID, Description, SQL, Format FROM Stat",
        QList<parameter>()
    );
}

queries::queryInfo* queries::getStatMapping()
{
    return new queryInfo(
        "SELECT PortfolioID, StatID FROM StatMapping ORDER BY PortfolioID, Sequence",
        QList<parameter>()
    );
}

queries::queryInfo* queries::updateSecurity(const int &portfolioID, const globals::security& sec)
{
    QList<parameter> params;
    params  << parameter(":PortfolioID", portfolioID)
            << parameter(":Symbol", sec.symbol)
            << parameter(":Account", sec.account)
            << parameter(":Expense", functions::doubleToNull(sec.expense))
            << parameter(":DivReinvest", (int)sec.divReinvest)
            << parameter(":CashAccount", (int)sec.cashAccount)
            << parameter(":IncludeInCalc", (int)sec.includeInCalc)
            << parameter(":Hide", (int)sec.hide);

    if(sec.id == -1) // insert new
    {
        return new queryInfo(
            "INSERT INTO Tickers (PortfolioID, Symbol, Account, Expense, DivReinvest, CashAccount, IncludeInCalc, Hide)"
            " VALUES (:PortfolioID, :Symbol, :Account, :Expense, :DivReinvest, :CashAccount, :IncludeInCalc, :Hide)",
            params
        );
    }
    else // update
    {
        params << parameter(":SecurityID", sec.id);
        return new queryInfo(
            "UPDATE Tickers SET PortfolioID = :PortfolioID, Symbol = :Symbol, Account = :Account, Expense = :Expense,"
                " DivReinvest = :DivReinvest, CashAccount = :CashAccount, IncludeInCalc = :IncludeInCalc, Hide = :Hide"
                " WHERE ID = :SecurityID",
            params
        );
    }
}

queries::queryInfo* queries::getSecurity()
{
    return new queryInfo(
        "SELECT ID, PortfolioID, Symbol, Account, Expense, DivReinvest, CashAccount,"
            " IncludeInCalc, Hide FROM Tickers ORDER BY PortfolioID",
        QList<parameter>()
    );
}

queries::queryInfo* queries::updateSecurityTrade(const int &tickerID, const globals::dynamicTrade& trade)
{
    QList<parameter> params;
    params  << parameter(":TickerID", tickerID)
            << parameter(":Type", (int)trade.tradeType)
            << parameter(":Value", trade.value)
            << parameter(":Price", functions::doubleToNull(trade.price))
            << parameter(":Commission", functions::doubleToNull(trade.commission))
            << parameter(":CashAccountID", functions::intToNull(trade.cashAccount))
            << parameter(":Frequency", (int)trade.frequency)
            << parameter(":Date", functions::dateToNull(trade.date))
            << parameter(":StartDate", functions::dateToNull(trade.startDate))
            << parameter(":EndDate", functions::dateToNull(trade.endDate));

    if(trade.id == -1) // insert new
    {
        return new queryInfo(
            "INSERT INTO TickersTrades (TickerID, Type, Value, Price, Commission, CashAccountID, Frequency, Date, StartDate, EndDate)"
            " VALUES (:TickerID, :Type, :Value, :Price, :Commission, :CashAccountID, :Frequency, :Date, :StartDate, :EndDate)",
            params
        );
    }
    else // update
    {
        params << parameter(":TradeID", trade.id);
        return new queryInfo(
            "UPDATE TickersTrades SET TickerID = :TickerID, Type = :Type, Value = :Value, Price = :Price,"
                " Commission = :Commission, CashAccountID = :CashAccountID, Frequency = :Frequency, Date = :Date"
                " StartDate = :StartDate, EndDate = :EndDate WHERE ID = :TradeID",
            params
        );
    }
}

queries::queryInfo* queries::getSecurityTrade()
{
    return new queryInfo(
        "SELECT a.ID, b.PortfolioID, a.TickerID, a.Type, a.Value, a.Price, a.Commission,"
            " a.CashAccountID, a.Frequency, a.Date, a.StartDate, a.EndDate"
        " FROM TickersTrades a"
        " INNER JOIN Tickers b"
            " ON a.TickerID = b.ID"
        " ORDER BY b.PortfolioID, a.TickerID",
        QList<parameter>()
    );
}

queries::queryInfo* queries::getSecurityAA()
{
    return new queryInfo(
        "SELECT b.PortfolioID, a.TickerID, a.AAID, a.Percent"
        " FROM TickersAA a"
        " INNER JOIN Tickers b"
            " ON a.TickerID = b.ID"
        " ORDER BY b.PortfolioID, a.TickerID",
        QList<parameter>()
    );
}

queries::queryInfo* queries::getUpdateInfo()
{
    return new queryInfo(
            "SELECT Ticker, MAX(Date) AS Date, 'C' AS Type"
            " FROM ClosingPrices"
            " GROUP BY Ticker"
        " UNION ALL "
            " SELECT Ticker, MAX(Date) as Date, 'D'"
            " FROM Dividends"
            " GROUP BY Ticker"
        " UNION ALL"
            " SELECT Ticker, MAX(Date) as Date, 'S'"
            " FROM Splits"
            " GROUP BY Ticker",
        QList<parameter>()
    );
}

queries::queryInfo* queries::updateMissingPrices()
{
    return new queryInfo(
        "INSERT INTO ClosingPrices (Ticker, Date, Price)"
        " SELECT a.Ticker, b.Date, d.Price"
        " FROM (SELECT Ticker, MIN(Date) AS MinDate, MAX(Date) as MaxDate from ClosingPrices GROUP BY Ticker ) a"
        " CROSS JOIN (SELECT DISTINCT Date FROM ClosingPrices) b"
        " LEFT JOIN ClosingPrices c"
                " ON a.Ticker = c.Ticker"
                " AND b.Date = c.Date"
        " LEFT JOIN ClosingPrices d"
                " ON a.Ticker = d.Ticker"
                " AND d.date = (SELECT MAX(e.Date) FROM ClosingPrices AS e WHERE e.Ticker = d.Ticker AND e.Date < b.Date)"
        " WHERE b.Date BETWEEN a.MinDate AND a.MaxDate AND c.Ticker IS NULL",
        QList<parameter>()
    );
}

queries::queryInfo* queries::getPortfolioLastDate()
{
    return new queryInfo(
            "SELECT PortfolioID, MAX(Date)"
            " FROM NAV "
            " GROUP BY PortfolioID",
        QList<parameter>()
    );
}

queries::queryInfo* queries::getPortfolioNAV(const int &portfolioID, const int &date)
{
    return new queryInfo(
            "SELECT PortfolioID, TotalValue, NAV"
            " FROM NAV "
            " WHERE PortfolioID = :PortfolioID AND Date = :Date",
        QList<parameter>()
            << parameter(":PortfolioID", portfolioID)
            << parameter(":Date", date)
    );
}

//queries::queryInfo* queries::deleteTrades(const int &portfolioID, const int &startingDate)
//{
//    return new queryInfo(
//            "DELETE FROM Trades"
//            " WHERE EXISTS"
//                " (SELECT 1 FROM Tickers WHERE Tickers.ID = Trades.TickerID AND Tickers.PortfolioID = :PortfolioID)"
//            " AND Date >= :Date",
//        QList<parameter>()
//            << parameter(":PortfolioID", portfolioID)
//            << parameter(":Date", startingDate)
//    );
//}
