//
// Created by LinjianLi on 2019/2/20.
//

#include "Separator.h"

Separator::Separator(set<Node*> set_node_ptr) {
  is_separator = true;
  weight = clique_size = set_node_ptr.size();
  if (weight!=0) {InitializeClique(set_node_ptr);}
}