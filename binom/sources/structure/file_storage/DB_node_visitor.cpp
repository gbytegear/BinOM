#include "binom/includes/structure/file_storage/DB_node_visitor.h"

using namespace binom;


NodeDescriptor DBNodeVisitor::loadNode(f_virtual_index node_index) const {return fvmc.loadNodeDescriptor(node_index);}
void DBNodeVisitor::updateNode() {node_descriptor = loadNode(node_index);}
ByteArray DBNodeVisitor::loadData() {return fvmc.loadDataByNode(node_index);}

f_virtual_index DBNodeVisitor::createVariable(Variable var) {
  switch (var.typeClass()) {
    case VarTypeClass::primitive: return createPrimitive(std::move(var.toPrimitive()));
    case VarTypeClass::buffer_array: return createBufferArray(std::move(var.toBufferArray()));
    case VarTypeClass::array: return createArray(std::move(var.toArray()));
    case VarTypeClass::object: return createObject(std::move(var.toObject()));
    default: throw SException(ErrCode::binom_invalid_type);
  }
}

f_virtual_index DBNodeVisitor::createPrimitive(Primitive primitive) {
  return fvmc.createNode(primitive.getType(),
                         ByteArray(primitive.getDataPtr(), toSize(primitive.getValType())));
}

f_virtual_index DBNodeVisitor::createBufferArray(BufferArray buffer_array) {
  return fvmc.createNode(buffer_array.getType(), buffer_array.toByteArray());
}

f_virtual_index DBNodeVisitor::createArray(Array array) {
  if(array.isEmpty())
    return fvmc.createNode(VarType::array, ByteArray());

  ByteArray array_data(array.getMemberCount() * sizeof (f_virtual_index));
  {
    f_virtual_index* index_it = array_data.begin<f_virtual_index>();
    for(Variable& var : array) {
      *index_it = createVariable(var);
      ++index_it;
    }
  }
  return fvmc.createNode(VarType::array, std::move(array_data));
}

f_virtual_index DBNodeVisitor::createObject(Object object) {
  if(object.isEmpty())
    return fvmc.createNode(VarType::object, ByteArray());
  ByteArray name_length_segment,
            name_segment,
            index_segment;
  ObjectNameLength name_length;
  for(NamedVariable& var : object) {
    ui64 symbol_count = var.name.getMemberCount();
    if(symbol_count != name_length.name_length) {
      if(object.begin() == &var) {
        name_length.name_length = symbol_count;
      } else {
        name_length_segment.pushBack(name_length);
        name_length.name_length = symbol_count;
        name_length.name_count = 0;
      }
    }

    name_segment += var.name.toByteArray();
    index_segment
        .pushBack<f_virtual_index>(
          createVariable(var.variable)
          );
    ++name_length.name_count;
  }

  name_length_segment.pushBack(name_length);

  ObjectDescriptor object_descriptor{
        name_length_segment.length<ObjectNameLength>(),
        name_segment.length(),
        index_segment.length<f_virtual_index>()
  };

  ByteArray data{
    ByteArray(&object_descriptor, sizeof (ObjectDescriptor)),
    name_length_segment,
    name_segment,
    index_segment
  };

  return fvmc.createNode(VarType::object, std::move(data));
}

void DBNodeVisitor::setPrimitive(f_virtual_index node_index, Primitive primitive) {
  clearNode(node_index);
  fvmc.updateNode(node_index, primitive.getType(),
                  ByteArray(primitive.getDataPtr(), toSize(primitive.getValType())));
}

void DBNodeVisitor::setBufferArray(f_virtual_index node_index, BufferArray buffer_array) {
  clearNode(node_index);
  if(buffer_array.isEmpty()) {
    fvmc.updateNode(node_index, buffer_array.getType(), ByteArray());
    return;
  }
  fvmc.updateNode(node_index, buffer_array.getType(), buffer_array.toByteArray());
}

void DBNodeVisitor::setArray(f_virtual_index node_index, Array array) {
  clearNode(node_index);
  if(array.isEmpty()) {
    fvmc.updateNode(node_index, VarType::array, ByteArray());
    return;
  }
  ByteArray array_data(array.getMemberCount() * sizeof (f_virtual_index));
  {
    f_virtual_index* index_it = array_data.begin<f_virtual_index>();
    for(Variable& var : array) {
      *index_it = createVariable(var);
      ++index_it;
    }
  }

  fvmc.updateNode(node_index, VarType::array, std::move(array_data));
}

void DBNodeVisitor::setObject(f_virtual_index node_index, Object object) {
  clearNode(node_index);
  if(object.isEmpty()) {
    fvmc.updateNode(node_index, VarType::object, ByteArray());
    return;
  }
  ByteArray name_length_segment,
            name_segment,
            index_segment;
  ObjectNameLength name_length;
  for(NamedVariable& var : object) {
    ui64 symbol_count = var.name.getMemberCount();
    if(symbol_count != name_length.name_length) {
      if(object.begin() == &var) {
        name_length.name_length = symbol_count;
      } else {
        name_length_segment.pushBack(name_length);
        name_length.name_length = symbol_count;
      }
    }

    name_segment += var.name.toByteArray();
    index_segment.pushBack<f_virtual_index>(createVariable(var.variable));
    ++name_length.name_count;
  }

  name_length_segment.pushBack(name_length);

  ObjectDescriptor object_descriptor{
        name_length_segment.length<ObjectNameLength>(),
        name_segment.length(),
        index_segment.length<f_virtual_index>()
  };

  ByteArray data{
    ByteArray(&object_descriptor, sizeof (ObjectDescriptor)),
    name_length_segment,
    name_segment,
    index_segment
  };

  fvmc.updateNode(node_index, VarType::object, std::move(data));
}

Variable DBNodeVisitor::buildVariable(f_virtual_index node_index) const {
  NodeDescriptor node_des(loadNode(node_index));
  switch (toTypeClass(node_des.type)) {
    case VarTypeClass::primitive: return std::move(buildPrimitive(node_des).asVar());
    case VarTypeClass::buffer_array: return std::move(buildBufferArray(node_des).asVar());
    case VarTypeClass::array: return std::move(buildArray(node_des).asVar());
    case VarTypeClass::object: return std::move(buildObject(node_des).asVar());
    default: throw SException(ErrCode::binom_invalid_type);
  }
}

Primitive DBNodeVisitor::buildPrimitive(NodeDescriptor node_des) const {
  ByteArray data(1 + toSize(toValueType(node_des.type)));
  data.set(0,0, node_des.type);
  data.set(1, fvmc.loadByteDataByIndex(node_des.index, toValueType(node_des.type)));
  void* ptr = data.unfree();
  return *reinterpret_cast<Primitive*>(&ptr);
}

BufferArray DBNodeVisitor::buildBufferArray(NodeDescriptor node_des) const {
  if(node_des.size == 0) return BufferArray(node_des.type);
  ByteArray data(9 + node_des.size);
  data.set(0,0, node_des.type);
  data.set(0,1, node_des.size/toSize(toValueType(node_des.type)));
  data.set(9, fvmc.loadHeapDataByIndex(node_des.index));
  void* ptr = data.unfree();
  return *reinterpret_cast<BufferArray*>(&ptr);
}

Array DBNodeVisitor::buildArray(NodeDescriptor node_des) const {
  if(node_des.size == 0) return Array();
  ByteArray data(9 + node_des.size);
  data.set(0,0, node_des.type);
  data.set(0, 1, node_des.size/8);
  ByteArray node_index_data(fvmc.loadHeapDataByIndex(node_des.index));

  for(ui64* var_index_it = data.begin<ui64>(9),
          * node_index_it = node_index_data.begin<ui64>();
      node_index_it != node_index_data.end<ui64>();
      (++var_index_it, ++node_index_it))
    new(var_index_it) Variable(buildVariable(*node_index_it));

  void* ptr = data.unfree();
  return *reinterpret_cast<Array*>(&ptr);
}

Object DBNodeVisitor::buildObject(NodeDescriptor node_des) const {
  if(node_des.size == 0) return Object();

  // Load data base object data
  ByteArray db_data(fvmc.loadHeapDataByIndex(node_des.index));
  ObjectDescriptor& object_descriptor = db_data.get<ObjectDescriptor>(0);

  // Create object buffer and set his properties
  ByteArray data(9 + object_descriptor.index_count * sizeof(NamedVariable));
  data.set<VarType>(0,0, VarType::object);
  data.set<ui64>(0,1,object_descriptor.index_count);

  // Iterators
  ObjectNameLength* nl_it = db_data.begin<ObjectNameLength>(sizeof(ObjectDescriptor));
  char* n_it = db_data.begin<char>(sizeof(ObjectDescriptor) +
                                object_descriptor.length_element_count*sizeof (ObjectNameLength));
  f_virtual_index* i_it = db_data.begin<f_virtual_index>(sizeof(ObjectDescriptor) +
                                                      object_descriptor.length_element_count*sizeof (ObjectNameLength) +
                                                      object_descriptor.name_block_size);
  NamedVariable* nvar_it = data.begin<NamedVariable>(9);

  // Copy of name length struct for calculate name length
  ObjectNameLength nl_c = *nl_it;

  while (!!object_descriptor.index_count) {

    // Build name buffer array
    ByteArray name_data(9 + nl_c.name_length);
    name_data.set<VarType>(0,0, VarType::byte_array);
    name_data.set<ui64>(0,1, nl_c.name_length);
    memcpy(name_data.begin() + 9, n_it, nl_c.name_length);
    void* n_ptr = name_data.unfree();

    // Create named variable by
    *reinterpret_cast<void**>(&nvar_it->name) = n_ptr;
    new(&nvar_it->variable) Variable(buildVariable(*i_it));

    // Iterator shifts
    ++i_it;
    n_it += nl_c.name_length;
    --nl_c.name_count;
    if(!nl_c.name_count)
      nl_c = *(++nl_it);
    ++nvar_it;

    --object_descriptor.index_count;
  }

  void* ptr = data.unfree();
  return *reinterpret_cast<Object*>(&ptr);
}

void DBNodeVisitor::clearNode(f_virtual_index node_index) {
  NodeDescriptor descriptor = fvmc.loadNodeDescriptor(node_index);
  switch (toTypeClass(descriptor.type)) {
    case VarTypeClass::primitive:
    case VarTypeClass::buffer_array:
    break;
    case VarTypeClass::array: {
      if(!descriptor.size) break;
      ByteArray data(fvmc.loadHeapDataByIndex(descriptor.index));
      for(f_virtual_index* index_it = data.begin<f_virtual_index>(); index_it != data.end<f_virtual_index>(); ++index_it)
        deleteNode(*index_it);
      break;
    }
    case VarTypeClass::object: {
      if(!descriptor.size) break;
      ByteArray data(fvmc.loadHeapDataByIndex(descriptor.index));
      ObjectDescriptor descriptor(data.get<ObjectDescriptor>(0));
      for(f_virtual_index* index_it = data.begin<f_virtual_index>(
            sizeof (ObjectDescriptor) +
            descriptor.length_element_count * sizeof (ObjectNameLength) +
            descriptor.name_block_size
            );
          index_it != data.end<f_virtual_index>();
          ++index_it)
        deleteNode(*index_it);
      break;
    }
    default: break;
  }
}

void DBNodeVisitor::deleteNode(f_virtual_index node_index) {
  clearNode(node_index);
  fvmc.free(node_index);
}

DBNodeVisitor::DBNodeVisitor(FileVirtualMemoryController& fvmc, f_virtual_index node_index)
  : fvmc(fvmc),
    node_index(node_index) {updateNode();}

DBNodeVisitor::DBNodeVisitor(const DBNodeVisitor& other)
  : fvmc(other.fvmc),
    node_descriptor(other.node_descriptor),
    node_index(other.node_index) {}

DBNodeVisitor::DBNodeVisitor(const DBNodeVisitor&& other)
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

DBNodeVisitor& DBNodeVisitor::stepInside(ui64 index) {
  if(isPrimitive() || isObject() || is_value_ptr) throw SException(ErrCode::binom_invalid_type);
  if(isArray()) {
    if(node_descriptor.size / sizeof (f_virtual_index) <= index) throw SException(ErrCode::binom_out_of_range);
    node_index = fvmc.loadHeapDataPartByIndex(node_descriptor.index,
                                              index*sizeof(f_virtual_index),
                                              sizeof (f_virtual_index))
                 .get<f_virtual_index>(0);
    updateNode();
  } else if(isBufferArray()) {
    if(node_descriptor.size / toSize(toValueType(getType())) >= index) throw SException(ErrCode::binom_out_of_range);
    value_index = index;
    is_value_ptr = true;
  }
  return *this;
}

DBNodeVisitor& DBNodeVisitor::stepInside(BufferArray name) {
  if(!isObject() || is_value_ptr) throw SException(ErrCode::binom_invalid_type);
  ByteArray data(loadData());
  ObjectDescriptor descriptor = data.get<ObjectDescriptor>(0);

  ui64 index = 0;

  {
    ObjectNameLength* length_it = data.begin<ObjectNameLength>(sizeof (ObjectDescriptor));

    i64 left = 0;
    i64 right = descriptor.length_element_count;
    i64 middle = 0;

    while(1) {
      middle = (left + right) / 2;
      if(left > right || middle > descriptor.length_element_count) {
        middle = -1;
        break;
      }

      if(length_it[middle].name_length > name.getMemberCount()) right = middle - 1;
      elif(length_it[middle].name_length < name.getMemberCount()) left = middle + 1;
      else break;
    }

    ui64 name_block_index = sizeof (ObjectDescriptor) +
                            descriptor.length_element_count*sizeof(ObjectNameLength);
    ui64 name_count = 0;

    if(middle == -1) throw SException(ErrCode::binom_out_of_range);

    for(ui64 i = 0; i < middle; ++i) {
      index += length_it[i].name_count;
      name_block_index += length_it[i].name_count*length_it[i].name_length;
    }
    name_count = length_it[middle].name_count;

    char* name_it = data.begin<char>(name_block_index);
    left = 0;
    right = name_count;
    middle = 0;

    while (1) {
      middle = (left + right) / 2;
      if(left > right || middle > name_count) {
        middle = -1;
        break;
      }

      int cmp = memcmp(name_it + name.getMemberCount()*middle, name.getDataPointer(), name.getMemberCount());

      if(cmp > 0) right = middle - 1;
      elif(cmp < 0) left = middle + 1;
      else break;
    }

    if(middle == -1) throw SException(ErrCode::binom_out_of_range);

    index += middle;
  }

  node_index = data.get<ui64>(index,
                              sizeof(ObjectDescriptor) +
                              descriptor.length_element_count*sizeof(ObjectNameLength) +
                              descriptor.name_block_size);
  updateNode();
  return *this;
}

Variable DBNodeVisitor::getVariable() const {
  return buildVariable(node_index);
}

Variable DBNodeVisitor::getVariable(ui64 index) const {
  return buildVariable(getChild(index).node_index);
}

Variable DBNodeVisitor::getVariable(BufferArray name) const {
  return buildVariable(getChild(name).node_index);
}

void DBNodeVisitor::setVariable(Variable var) {
  if(!node_index) fvmc.clear();
  else fvmc.markNodeAsBusy(node_index);

  switch (var.typeClass()) {
    case VarTypeClass::primitive:
      setPrimitive(node_index, std::move(var.toPrimitive()));
    break;
    case VarTypeClass::buffer_array:
      setBufferArray(node_index, std::move(var.toBufferArray()));
    break;
    case VarTypeClass::array:
      setArray(node_index, std::move(var.toArray()));
    break;
    case VarTypeClass::object:
      setObject(node_index, std::move(var.toObject()));
    break;
    default: throw SException(ErrCode::binom_invalid_type);
  }

  updateNode();
}

void DBNodeVisitor::pushBack(Variable var) {
  switch (getTypeClass()) {
    default:
      throw SException(ErrCode::binom_invalid_type);
    case VarTypeClass::array: {
      ByteArray array_data(loadData());
      array_data.pushBack(createVariable(var));
      fvmc.freeNodeData(node_index);
      fvmc.updateNode(node_index, VarType::array, std::move(array_data));
      break;
    }
    case VarTypeClass::buffer_array: {
      if((!var.isPrimitive() && !var.isBufferArray()) ||
         toValueType(var.type()) != toValueType(var.type()))
        throw SException(ErrCode::binom_invalid_type);
      ByteArray array_data(loadData());
      if(var.isPrimitive())
        array_data.pushBack(var.toPrimitive().getDataPtr(), toSize(toValueType(var.type())));
      elif(var.isBufferArray())
        array_data.pushBack(var.toBufferArray().getDataPointer(),
                            var.toBufferArray().getMemberCount()*var.toBufferArray().getMemberSize());
      fvmc.freeNodeData(node_index);
      fvmc.updateNode(node_index, node_descriptor.type, std::move(array_data));
      break;
    }
  }
  updateNode();
}

void DBNodeVisitor::pushFront(Variable var) {
  switch (getTypeClass()) {
    default:
      throw SException(ErrCode::binom_invalid_type);
    case VarTypeClass::array: {
      ByteArray array_data(loadData());
      array_data.pushFront(createVariable(var));
      fvmc.freeNodeData(node_index);
      fvmc.updateNode(node_index, VarType::array, std::move(array_data));
     break;
    }
    case VarTypeClass::buffer_array: {
      if((!var.isPrimitive() && !var.isBufferArray()) ||
         toValueType(var.type()) != toValueType(var.type()))
        throw SException(ErrCode::binom_invalid_type);
      ByteArray array_data(loadData());
      if(var.isPrimitive())
        array_data.pushFront(var.toPrimitive().getDataPtr(), toSize(toValueType(var.type())));
      elif(var.isBufferArray())
        array_data.pushFront(var.toBufferArray().getDataPointer(),
                            var.toBufferArray().getMemberCount()*var.toBufferArray().getMemberSize());
      fvmc.freeNodeData(node_index);
      fvmc.updateNode(node_index, node_descriptor.type, std::move(array_data));
      break;
    }
  }
  updateNode();
}

void DBNodeVisitor::insert(ui64 index, Variable var) {
  switch (getTypeClass()) {
    default:
      throw SException(ErrCode::binom_invalid_type);
    case VarTypeClass::array: {
      ByteArray array_data(loadData());
      array_data.insert<f_virtual_index>(index, 0, createVariable(var));
      fvmc.freeNodeData(node_index);
      fvmc.updateNode(node_index, VarType::array, std::move(array_data));
      break;
    }
    case VarTypeClass::buffer_array: {
      if((!var.isPrimitive() && !var.isBufferArray()) ||
         toValueType(var.type()) != toValueType(var.type()))
        throw SException(ErrCode::binom_invalid_type);
      ByteArray array_data(loadData());
      if(var.isPrimitive())
        array_data.insert(index, var.toPrimitive().getDataPtr(), toSize(toValueType(var.type())));
      elif(var.isBufferArray())
        array_data.insert(index, var.toBufferArray().getDataPointer(),
                                 var.toBufferArray().getMemberCount()*var.toBufferArray().getMemberSize());
      fvmc.freeNodeData(node_index);
      fvmc.updateNode(node_index, node_descriptor.type, std::move(array_data));
      break;
    }
  }
  updateNode();
}

void DBNodeVisitor::insert(BufferArray name, Variable var) {

  /* NOTE: void insert(BufferArray, Variable)
   * // Псевдокод этого днища, чтобы хотябы понимать что я тварю
   * // Стоило бы так расписать все функции работы с BinOM-объектами в базе данных
   * // ибо иначе будет модификация реврайтом с нуля - что ни есть хорошо
   * // FOR_REFCATOR: стоило бы лучше формализировать
   * //               нейминг логики сокрытых частей классов
   *
   * если неправильный тип данных {кинуть исключение (неправильный тип данных);}
   * создать буффер из (загрузить данные по дескриптору узла(номер текущего дескриптора));
   *
   * если буфер путсой то { // aka пустой BinOM-объект
   *   пересоздать буфер размером (размер дескриптора объекта +
   *                                размер блока размера имени +
   *                                размер имени +
   *                                размер индекса);
   *   создать дескриптор объекта{
   *     колличество блоков размера имен: 1
   *     размер блока имени: размер имени
   *     колличество индексов: 1
   *   };
   *   создать блок размера имени {
   *     колличество имён: 1
   *     размер имени: размер имени
   *   };
   *   индекс переменной = создать переменную в базе(переменная);
   *   записать в буфер в индекс(0) данные из (дескриптор);
   *   записать в буфер в индекс(размер дескриптора объекта) данные из (блок размера имени);
   *   записать в буфер в индекс(размер дескриптора объекта +
   *                              размер блока размера имени) данные из (имя);
   *   записать в буфер в индекс(размер дескриптора объекта +
   *                              размер блока размера имени +
   *                              размер имени) данные из (индекс переменной);
   *   обновить дескриптор узла номер(номер текущего дескриптора) данными из (буфер)
   *
   * } иначе { // Не пустой буфер
   *   ссылка на дескриптор = буфер.взять<дескриптор объекта> по индексу(0);
   *   итератор блоков размера = буфер.начать<Блок Размера Имени>
   *                             по индексу (размер дескриптора объекта);
   *   итератор имён = буфер.начать<символ>
   *                   по индексу (размер дескриптора объекта +
   *                               размер блока размера имени * дескриптор объекта.колличество блоков размера имени);
   *   итератор индексов = буфер.начать<виртуальный индекс>
   *                       по индексу (размер дескриптора объекта +
   *                                   размер блока размера имени * дескриптор объекта.колличество блоков размера имени +
   *                                   дескриптор объекта.размер блока имён);
   *
   *   // Использовать бинарный поиск для нахождения
   *   // нужного блока размера (или позиции где он должен быть) бесполезно
   *   // поскольку всё-равно нужно будет подсчитать
   *   // сдвиг до блока имён, а это без вариантов O(n)
   *   // (переписать логику в DBNodeVisitor::stepInside поскольку там
   *   //  та же ситуция с асмптотической сложностью)
   *
   *   // Оптимизировать это нужно на уровне структуры BinOM-объектов в базе данных
   *   // пока-что я слишком тупой для того чтобы придумать что-то лучше \_(-_-)_/
   *   // (B-деревья не особо подходят ибо увиличится сложность добавлений/удалений элементов)
   *
   *   индекс позиции блока размера = 0;
   *   совпадение размеров = ложь;
   *   сдвиг до блока имён = 0;
   *   перебрать от 0 ДО(включительно) дескриптор объекта.колличество блоков размера имени {
   *     если итератор блоков размера[i].размер имени РАВЕН имя.размер ТО {
   *       индекс позиции блока размера = i;
   *       совпадение размеров = истина;
   *       выйти из цикла;
   *     } иначе если i РАВНО дескриптор объекта.колличество блоков размера ИЛИ
   *                  итератор блоков размера[i].размер имени МЕНЬШЕ имя.размер ТО {
   *       индекс позиции блока размера = i;
   *       выйти из цикла;
   *     }
   *     сдвиг до блока имён = итератор блоков размера[i].размер имени *
   *                           итератор блоков размера[i].колличество имён;
   *   }
   *
   *   если совпадение размеров то {
   *     левая граница = 0;
   *     правая граница = итератор блоков размера[i].колличество имён;
   *     середина = 0;
   *
   *     бесконечный цикл { // Дописать бинарный посик по именам в блоке имён
   *       середина = (левая граница + правая граница) / 2;
   *       если левая граница больше правой то {выйти из цикла}
   *
   *       // Через memcmp...
   *       если БОЛЬШЕ имя то {правая граница = середина - 1;}
   *       иначе если МЕНЬШЕ имя то {левая граница = середина + 1;}
   *       иначе {выйти из цикла}
   *     }
   *
   *     // Дописать потом...
   *
   *   } иначе { // не совпадение размеров
   *
   *     // Дописать потом...
   *
   *   }
   *
   *   } // Конец условия: Не пустой буфер
   *  обновить данные локального дескриптора(номер текущего дескриптора);
  */


  if(getTypeClass() != VarTypeClass::object)
    throw SException(ErrCode::binom_invalid_type);
  ByteArray object_data(loadData());
  if(object_data.isEmpty()) { // If is empty object
    object_data.reset(sizeof(ObjectDescriptor) +
                      sizeof (ObjectNameLength) +
                      name.getMemberCount() +
                      sizeof (f_virtual_index));

    ObjectDescriptor descriptor {
      1,
      name.getMemberCount(),
      1
    };

    ObjectNameLength name_length_block {
      name.getMemberCount(),
      1
    };

    f_virtual_index var_index = createVariable(std::move(var));

    object_data.set(0, 0, descriptor);
    object_data.set(0, sizeof(ObjectDescriptor), name_length_block);
    object_data.set(sizeof(ObjectDescriptor) + sizeof (ObjectNameLength), name.toByteArray());
    object_data.set(0, sizeof(ObjectDescriptor) + sizeof (ObjectNameLength) + name.getMemberCount(), var_index);

    fvmc.updateNode(node_index, VarType::object, std::move(object_data));

  } else {
    ObjectDescriptor& descriptor = object_data.get<ObjectDescriptor>(0);

    ObjectNameLength* length_block_it = object_data.begin<ObjectNameLength>(sizeof(ObjectDescriptor));
    char* name_it = object_data.begin<char>(sizeof(ObjectDescriptor) +
                                            sizeof(ObjectNameLength)*descriptor.length_element_count);
    f_virtual_index* index_it = object_data.begin<f_virtual_index>(sizeof(ObjectDescriptor) +
                                                                   sizeof(ObjectNameLength)*descriptor.length_element_count +
                                                                   descriptor.name_block_size);

    ui64 length_block_pos = 0;
    bool length_cmp = false;
    for(ui64 i = 0; i <= descriptor.length_element_count; ++i) {
      if(i == descriptor.length_element_count ||
         length_block_it[i].name_length < name.getMemberCount()) {
        length_block_pos = i;
        break;
      } elif(length_block_it[i].name_length == name.getMemberCount()) {
        length_block_pos = i;
        length_cmp = true;
        break;
      }
      name_it += length_block_it[i].name_length*length_block_it[i].name_count;
    }

    length_block_it += length_block_pos;

    if(length_cmp) {

      i64 left = 0,
          right = length_block_it->name_count,
          middle = 0;

      while (1) {
        middle = (left + right)/2;
        if(left > right) break;

        int cmp = memcmp(name_it + name.getMemberCount()*middle, name.getDataPointer(), name.getMemberCount());

        if(cmp > 0) right = middle-1;
        elif(cmp < 0) left = middle+1;
        else throw SException(ErrCode::binom_object_key_error);
      }

      name_it +=
          (memcmp(name_it + name.getMemberCount()*middle,
                  name.getDataPointer(),
                  name.getMemberCount()) > 0)
          ? name.getMemberCount()*(middle+1)
          : name.getMemberCount()*middle;

      // Insertion
      object_data.insert(name_it, name.getDataPointer(), name.getMemberCount());
      ++length_block_it->name_count;
      object_data.insert<f_virtual_index>(index_it, createVariable(var));
    } else {
      object_data.insert(name_it, name.getDataPointer(), name.getMemberCount());
      object_data.insert<ObjectNameLength>(length_block_it, ObjectNameLength{name.getMemberCount(),1});
      object_data.insert<f_virtual_index>(index_it, createVariable(var));
    }

    fvmc.updateNode(node_index, VarType::object, std::move(object_data));

  }
  updateNode();
}

void DBNodeVisitor::remove(ui64 index) {
  switch (getTypeClass()) {
    default:
      throw SException(ErrCode::binom_invalid_type);
    case VarTypeClass::array: {
      ByteArray array_data(loadData());
      f_virtual_index delete_node_index = array_data.get<f_virtual_index>(index);
      deleteNode(delete_node_index);
      array_data.remove<f_virtual_index>(index, 0);
      fvmc.freeNodeData(node_index);
      fvmc.updateNode(node_index, VarType::array, std::move(array_data));
      break;
    }
    case VarTypeClass::buffer_array: {
      ByteArray array_data(loadData());
      array_data.remove(index*toSize(toValueType(getType())), toSize(toValueType(getType())));
      fvmc.freeNodeData(node_index);
      fvmc.updateNode(node_index, node_descriptor.type, std::move(array_data));
      break;
    }
  }
  updateNode();
}

DBNodeVisitor DBNodeVisitor::getChild(ui64 index) const {return DBNodeVisitor(*this).stepInside(index);}
DBNodeVisitor DBNodeVisitor::getChild(BufferArray name) const {return DBNodeVisitor(*this).stepInside(std::move(name));}

DBNodeVisitor DBNodeVisitor::operator[](ui64 index) {return DBNodeVisitor(*this).stepInside(index);}
DBNodeVisitor DBNodeVisitor::operator[](BufferArray name) {return DBNodeVisitor(*this).stepInside(std::move(name));}

DBNodeVisitor& DBNodeVisitor::operator()(ui64 index) {return stepInside(index);}
DBNodeVisitor& DBNodeVisitor::operator()(BufferArray name) {return stepInside(std::move(name));}
