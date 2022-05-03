#include "libbinom/include/variables/variable.hxx"

using namespace binom;
using namespace binom::priv;

SharedResource::~SharedResource() {Variable::destroyResorce(resource_data);}
