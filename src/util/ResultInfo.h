#ifndef _CResultInfo_h_
#define _CResultInfo_h_

class ResultInfo
{
private:
    double m_maxN;
    double m_minN;
    double m_maxT;
    double m_minT;
    double m_maxM;
    double m_minM;
    double m_maxV;
    double m_minV;
    double m_maxNavier;
    double m_minNavier;

public:
    ResultInfo();
    virtual ~ResultInfo();

    void setMaxM(double maxM);
    double getMaxV();
    void setMaxV(double maxV);
    double getMaxT();
    void setMaxT(double maxT);
    double getMaxN();
    void setMaxN(double maxN);
    double getMaxM();
    void setMaxNavier(double maxNavier);
    double getMaxNavier();

    void setMinM(double minM);
    double getMinV();
    void setMinV(double minV);
    double getMinT();
    void setMinT(double minT);
    double getMinN();
    void setMinN(double minN);
    double getMinM();
    void setMinNavier(double minNavier);
    double getMinNavier();
};

#endif
