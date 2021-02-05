#include "binom/includes/structure/file_storage/DB_node_visitor.h"

using namespace binom;


NodeDescriptor DBNodeVisitor::loadNode(f_virtual_index node_index) {return fvmc.loadNodeDescriptor(node_index);;}
void DBNodeVisitor::updateNode() {node_descriptor = loadNode(node_index);}
ByteArray DBNodeVisitor::loadData() {return fvmc.loadData(node_index);}

DBNodeVisitor::DBNodeVisitor(FileVirtualMemoryController& fvmc, f_virtual_index node_index)
  : fvmc(fvmc),
    node_index(node_index) {updateNode();}

DBNodeVisitor::DBNodeVisitor(DBNodeVisitor& other)
  : fvmc(other.fvmc),
    node_descriptor(other.node_descriptor),
    node_index(other.node_index) {}

DBNodeVisitor::DBNodeVisitor(DBNodeVisitor&& other)
  : fvmc(other.fvmc),
    node_descriptor(other.node_descriptor),
    node_index(other.node_index) {}

DBNodeVisitor& DBNodeVisitor::operator=(DBNodeVisitor other) {return *(new(this) DBNodeVisitor(other));}
DBNodeVisitor& DBNodeVisitor::operator=(f_virtual_index _node_index) {node_index = _node_index; updateNode(); return *this;}

VarType DBNodeVisitor::getType() {return node_descriptor.type;}
VarTypeClass DBNodeVisitor::getTypeClass() {return toTypeClass(getType());}

bool DBNodeVisitor::isPrimitive() {return getTypeClass() == VarTypeClass::primitive;}
bool DBNodeVisitor::isBufferArray() {return getTypeClass() == VarTypeClass::buffer_array;}
bool DBNodeVisitor::isArray() {return getTypeClass() == VarTypeClass::array;}
bool DBNodeVisitor::isObject() {return getTypeClass() == VarTypeClass::object;}

DBNodeVisitor DBNodeVisitor::getChild(ui64 index) {return DBNodeVisitor(*this).stepInside(index);}
DBNodeVisitor DBNodeVisitor::getChild(BufferArray name) {return DBNodeVisitor(*this).stepInside(std::move(name));}

DBNodeVisitor DBNodeVisitor::operator[](ui64 index) {return DBNodeVisitor(*this).stepInside(index);}
DBNodeVisitor DBNodeVisitor::operator[](BufferArray name) {return DBNodeVisitor(*this).stepInside(std::move(name));}

DBNodeVisitor& DBNodeVisitor::operator()(ui64 index) {return stepInside(index);}
DBNodeVisitor& DBNodeVisitor::operator()(BufferArray name) {return stepInside(std::move(name));}
