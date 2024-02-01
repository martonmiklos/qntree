
#include "qntree.h"

#include <TableSet>

#ifdef NUT_NAMESPACE
using namespace NUT_NAMESPACE;
#endif

Qntree::Qntree()
  : Database(),
  m_suppliers(new Nut::TableSet<Suppliers>(this)),
  m_supplier_parameter_map(new Nut::TableSet<SupplierParameterMap>(this)),
  m_supplier_category_map(new Nut::TableSet<SupplierCategoryMap>(this))
{
  qRegisterMetaType<Suppliers*>();
  qRegisterMetaType<SupplierParameterMap*>();
  qRegisterMetaType<SupplierCategoryMap*>();
}



