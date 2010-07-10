#include "portfolio.h"

class portfolioData: public QSharedData
{
public:
    QMap<int, security> securities;
    QMap<int, assetAllocation> aa;
    QMap<int, account> acct;
    executedTradeList executedTrades;
    navInfoPortfolio nav;
    portfolioInfo info;

    portfolioData(const int &id_):
            executedTrades(executedTradeList(id_)),
            info(portfolioInfo(id_)),
            nav(navInfoPortfolio(id_))
    {}
};

portfolio::portfolio(int id_):
    d(new portfolioData(id_))
{
}

portfolio::portfolio(const portfolio &other):
    d(other.d)
{
}

portfolio::~portfolio()
{
}

portfolio& portfolio::operator=(const portfolio &other)
{
    d = other.d;
    return *this;
}

//portfolio::portfolio()
//{
//
//#ifdef CLOCKTIME
//    QTime t;
//    t.start();
//#endif
//
//    loadPortfoliosInfo();
//    loadPortfoliosAA();
//    loadPortfoliosAcct();
//    loadPortfoliosSecurity();
//    loadPortfoliosSecurityAA();
//    loadPortfoliosSecurityTrades();
//    loadPortfoliosExecutedTrades();
//    loadPortfoliosNAV();
//
//#ifdef CLOCKTIME
//    qDebug("Time elapsed: %d ms (portfolio)", t.elapsed());
//#endif
//}
//
//int portfolio::minimumDate(const int &currentMinimumDate, const int &date) const
//{
//    int returnDate = currentMinimumDate;
//    if (date != -1 && (date < currentMinimumDate || currentMinimumDate == -1))
//        returnDate = date;
//
//    return returnDate;
//}
//
//int portfolio::minimumDate(const int &currentMinimumDate, const int &portfolioID, const assetAllocation &aa) const
//{
//    int returnDate = currentMinimumDate;
//    foreach(const security &s, securities(portfolioID))
//        if(s.aa.contains(aa.id))
//            foreach(const trade &t, s.trades)
//                if (t.type == trade::tradeType_AA)
//                {
//                    returnDate = minimumDate(currentMinimumDate, s.firstTradeDate());
//                    break;
//                }
//
//    return returnDate;
//}
//
//bool portfolio::datesOutsidePriceData() const
//{
//    int firstDate = prices::instance().firstDate();
//    if (firstDate == 0)
//        return true;
//
//    foreach(const portfolioData &d, m_portfolios)
//        if (!d.nav.isEmpty() && d.nav.firstDate() < firstDate)
//            return true;
//
//    return false;
//}

void portfolio::loadPortfoliosInfo()
{
    QSqlQuery q = queries::select(queries::table_Portfolios, queries::portfoliosColumns);
    while(q.next())
    {
        portfolioInfo info;

        info.id = q.value(queries::portfoliosColumns_ID).toInt();
        info.description = q.value(queries::portfoliosColumns_Description).toString();
        info.startDate = q.value(queries::portfoliosColumns_StartDate).toInt();
        info.dividends = q.value(queries::portfoliosColumns_Dividends).toBool();
        info.costBasis = (account::costBasisType)q.value(queries::portfoliosColumns_CostBasis).toInt();
        info.startValue = q.value(queries::portfoliosColumns_StartValue).toInt();
        info.aaThreshold = q.value(queries::portfoliosColumns_AAThreshold).toInt();
        info.aaThresholdMethod = (portfolioInfo::thesholdMethod)q.value(queries::portfoliosColumns_ThresholdMethod).toInt();
        info.holdingsShowHidden = q.value(queries::portfoliosColumns_HoldingsShowHidden).toBool();
        info.navSortDesc = q.value(queries::portfoliosColumns_NAVSortDesc).toBool();
        info.aaShowBlank = q.value(queries::portfoliosColumns_AAShowBlank).toBool();
        info.correlationShowHidden = q.value(queries::portfoliosColumns_CorrelationShowHidden).toBool();
        info.acctShowBlank = q.value(queries::portfoliosColumns_AcctShowBlank).toBool();
        info.holdingsSort = q.value(queries::portfoliosColumns_HoldingsSort).toString();
        info.aaSort = q.value(queries::portfoliosColumns_AASort).toString();
        info.acctSort = q.value(queries::portfoliosColumns_AcctSort).toString();

        insert(info);
    }
}

void portfolio::loadPortfoliosAA()
{
    QSqlQuery q = queries::select(queries::table_AA, queries::aaColumns);
    while(q.next())
    {
        assetAllocation aa;

        aa.id = q.value(queries::aaColumns_ID).toInt();
        aa.description = q.value(queries::aaColumns_Description).toString();
        if (!q.value(queries::aaColumns_Target).isNull())
            aa.target = q.value(queries::aaColumns_Target).toDouble();

        insert(q.value(queries::aaColumns_PortfolioID).toInt(), aa);
    }
}

void portfolio::loadPortfoliosAcct()
{
    QSqlQuery q = queries::select(queries::table_Acct, queries::acctColumns);
    while(q.next())
    {
        account acct;

        acct.id = q.value(queries::acctColumns_ID).toInt();
        acct.description = q.value(queries::acctColumns_Description).toString();
        if (!q.value(queries::acctColumns_TaxRate).isNull())
            acct.taxRate = q.value(queries::acctColumns_TaxRate).toDouble();
        acct.taxDeferred = q.value(queries::acctColumns_TaxDeferred).toBool();
        acct.costBasis = (account::costBasisType)q.value(queries::acctColumns_CostBasis).toInt();

        insert(q.value(queries::acctColumns_PortfolioID).toInt(), acct);
    }
}

void portfolio::loadPortfoliosSecurity()
{
    QSqlQuery q = queries::select(queries::table_Security, queries::SecurityColumns);
    while(q.next())
    {
        security sec;

        sec.id = q.value(queries::securityColumns_ID).toInt();
        sec.description = q.value(queries::securityColumns_Symbol).toString();
        if (!q.value(queries::securityColumns_Account).isNull())
            sec.account = q.value(queries::securityColumns_Account).toInt();
        if (!q.value(queries::securityColumns_Expense).isNull())
            sec.expense = q.value(queries::securityColumns_Expense).toDouble();
        sec.divReinvest = q.value(queries::securityColumns_DivReinvest).toBool();
        sec.cashAccount = q.value(queries::securityColumns_CashAccount).toBool();
        if (sec.cashAccount)
            prices::instance().insertCashSecurity(sec.description);
        sec.includeInCalc = q.value(queries::securityColumns_IncludeInCalc).toBool();
        sec.hide = q.value(queries::securityColumns_Hide).toBool();

        insert(q.value(queries::securityColumns_PortfolioID).toInt(), sec);
    }
}

void portfolio::loadPortfoliosSecurityAA()
{
    QSqlQuery q = queries::select(queries::table_SecurityAA, queries::SecurityAAColumns, QString(), true);
    while(q.next())
        insertAAPercentage(q.value(queries::securityAAColumns_Count).toInt(), q.value(queries::securityAAColumns_SecurityID).toInt(),
            q.value(queries::securityAAColumns_AAID).toInt(), q.value(queries::securityAAColumns_Percent).toDouble());
}

void portfolio::loadPortfoliosSecurityTrades()
{
    QSqlQuery q = queries::select(queries::table_SecurityTrades, queries::SecurityTradeColumns, QString(), true);
    while(q.next())
    {
        trade t;

        t.id = q.value(queries::securityTradeColumns_ID).toInt();
        t.type = (trade::tradeType)q.value(queries::securityTradeColumns_Type).toInt();
        t.value = q.value(queries::securityTradeColumns_Value).toDouble();
        if (!q.value(queries::securityTradeColumns_Price).isNull())
            t.price = q.value(queries::securityTradeColumns_Price).toDouble();
        t.commission = q.value(queries::securityTradeColumns_Commission).toDouble();
        if (!q.value(queries::securityTradeColumns_CashAccountID).isNull())
            t.cashAccount = q.value(queries::securityTradeColumns_CashAccountID).toInt();
        t.frequency = (trade::tradeFreq)q.value(queries::securityTradeColumns_Frequency).toInt();
        if (!q.value(queries::securityTradeColumns_Date).isNull())
            t.date = q.value(queries::securityTradeColumns_Date).toInt();
        if (!q.value(queries::securityTradeColumns_StartDate).isNull())
            t.startDate = q.value(queries::securityTradeColumns_StartDate).toInt();
        if (!q.value(queries::securityTradeColumns_EndDate).isNull())
            t.endDate = q.value(queries::securityTradeColumns_EndDate).toInt();

        insertTrade(q.value(queries::securityTradeColumns_Count).toInt(), q.value(queries::securityTradeColumns_SecurityID).toInt(), t);
    }
}

void portfolio::loadPortfoliosExecutedTrades()
{
    QSqlQuery q = queries::select(queries::table_ExecutedTrades, queries::executedTradesColumns,
        queries::executedTradesColumns.at(queries::executedTradesColumns_Date), true);

    while(q.next())
        insertExecutedTrade(q.value(queries::executedTradesColumns_Count).toInt(), q.value(queries::executedTradesColumns_SecurityID).toInt(),
            executedTrade(
                q.value(queries::executedTradesColumns_Date).toInt(),
                q.value(queries::executedTradesColumns_Shares).toDouble(),
                q.value(queries::executedTradesColumns_Price).toDouble(),
                q.value(queries::executedTradesColumns_Commission).toDouble()
            )
        );
}

void portfolio::loadPortfoliosNAV()
{
    QSqlQuery q = queries::select(queries::table_NAV, queries::navColumns);
    while(q.next())
        insertNAV(q.value(queries::navColumns_PortfolioID).toInt(), q.value(queries::navColumns_Date).toInt(),
            q.value(queries::navColumns_NAV).toDouble(), q.value(queries::navColumns_TotalValue).toDouble());
}

QStringList portfolio::symbols() const
{
    QStringList list;
    foreach(const security &s, d->securities)
        if (!s.cashAccount)
            list.append(s.description);

    list.removeDuplicates();
    return list;
}

const QList<int> portfolio::securityReinvestments()
{
    QList<int> reinvestments;
    foreach(const security &s, d->securities)
        if (s.includeInCalc && s.divReinvest && !s.cashAccount)
            reinvestments.append(s.id);

    return reinvestments;
}

void portfolio::remove(const queries &dataSource_)
{
    dataSource_.deleteItem(queries::table_Portfolios, d->info.id);
}

void portfolio::remove(const queries &dataSource_, const assetAllocation &aa_)
{
    aa.remove(dataSource_);
    d->aa.remove(aa.id);
    for(QMap<int, security>::iterator i = d->securities.begin(); i != d->securities.end(); ++i)
        i->removeAATarget(aa.id);
}

void portfolio::remove(const queries &dataSource_, const account &acct_)
{
    acct.remove(dataSource_);
    d->acct.remove(acct.id);
    for(QMap<int, security>::iterator i = d->securities.begin(); i != d->securities.end(); ++i)
        i->removeAccount(acct.id, portfolioID);
}

void portfolio::remove(const queries &dataSource_, const security &security_)
{
    sec.remove(dataSource_);
    d->securities.remove(sec.id);
}
