#ifndef __OUTPOST_TOOLS_H_
#define __OUTPOST_TOOLS_H_


class OutpostTools {
 public:
    static double GetBuyRateAfterTime(unsigned int minutes, double current_price);
    static double GetSellRateAfterTime(unsigned int minutes, double current_price);
    static double GetBuyRateAfterPurchase(unsigned int stock, double current_price);
    static double GetSellRateAfterSale(unsigned int stock, double current_price);
};

#endif
