//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// property_enforcer.cpp
//
// Identification: src/optimizer/property_enforcer.cpp
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "optimizer/property_enforcer.h"
#include "optimizer/operators.h"
#include "optimizer/property.h"
#include "optimizer/properties.h"
#include "expression/expression_util.h"

namespace peloton {
namespace optimizer {

std::shared_ptr<GroupExpression> PropertyEnforcer::EnforceProperty(
    std::shared_ptr<GroupExpression> gexpr, PropertySet *properties,
    std::shared_ptr<Property> property) {
  input_gexpr_ = gexpr;
  input_properties_ = properties;
  property->Accept(this);
  return output_gexpr_;
}

void PropertyEnforcer::Visit(const PropertyColumns *) {}

void PropertyEnforcer::Visit(const PropertyProjection *) {
  std::vector<GroupID> child_groups(1, input_gexpr_->GetGroupID());
  
  output_gexpr_ =
      std::make_shared<GroupExpression>(PhysicalProject::make(), child_groups);
}

void PropertyEnforcer::Visit(const PropertySort *property) {
  std::vector<expression::AbstractExpression *> sort_columns;
  std::vector<bool> sort_ascending;
  size_t column_size = property->GetSortColumnSize();
  for (size_t i = 0; i < column_size; ++i) {
    sort_columns.push_back(property->GetSortColumn(i));
    sort_ascending.push_back(property->GetSortAscending(i));
  }

  std::vector<GroupID> child_groups(1, input_gexpr_->GetGroupID());
  output_gexpr_ = std::make_shared<GroupExpression>(
      PhysicalOrderBy::make(sort_columns, sort_ascending), child_groups);
}

void PropertyEnforcer::Visit(const PropertyPredicate *) {}

} /* namespace optimizer */
} /* namespace peloton */
