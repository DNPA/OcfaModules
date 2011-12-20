#include <facade/EvidenceDeriveAccessor.hpp>

class tcpflowDissector:public ocfa::facade::EvidenceDeriveAccessor {
public:
  tcpflowDissector();
  void processEvidence();
private:
};

