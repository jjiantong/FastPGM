//
// Created by LinjianLi on 2019/2/20.
//

#include "Separator.h"

Separator::Separator(set<Node*> set_node_ptr) {
  weight = set_node_ptr.size();
  Clique(set_node_ptr);
}