
#ifdef ITERATOR_CLASS
template<typename Type>
class template_iterator {
  Type* ptr;
public:
  template_iterator(void* ptr) : ptr(reinterpret_cast<Type*>(ptr)) {}
  template_iterator(template_iterator&& other) : ptr(other.ptr) {}
  template_iterator& operator++() {++ptr; return *this;}
  template_iterator& operator--() {--ptr; return *this;}
  template_iterator operator++(int) {return ptr++;}
  template_iterator operator--(int) {return ptr--;}
  Type& operator*() const {return *ptr;}
};
#undef ITERATOR_CLASS
#endif

#ifdef ITERATOR_FUNCS

template<typename Type>
template_iterator<Type> tbegin() {return data_ptr;}
template<typename Type>
template_iterator<Type> tend() {return data_ptr + ui64(trunc(double(count) / sizeof (Type)));}

template<typename Type>
const template_iterator<Type> ctbegin() {return data_ptr;}
template<typename Type>
const template_iterator<Type> ctend() {return data_ptr + ui64(trunc(double(count) / sizeof (Type)));}

#undef ITERATOR_FUNCS
#endif
