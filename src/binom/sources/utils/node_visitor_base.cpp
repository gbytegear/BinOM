#include "binom/includes/utils/node_visitor.h"
#include "binom/includes/file_storage/file_node_visitor.h"

binom::Variable binom::NodeVisitorBase::getVariableClone() {
  return ((getVisitorType() == VisitorType::ram_storage_visitor)
          ? toRAMVisitor().getVariable()
          : toFileVisitor().getVariable());
}
