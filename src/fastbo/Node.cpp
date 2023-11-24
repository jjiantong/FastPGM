#include <utility>

//
// Created by Linjian Li on 2018/11/29.
//

#include "fastbo/Node.h"
#include "fastbo/DiscreteNode.h"
#include "fastbo/ContinuousNode.h"

Node::Node(int index) : Node(index, to_string(index)){

}

Node::Node(int index, string name) {
  SetNodeIndex(index);
  node_name = std::move(name);
}

int Node::GetNodeIndex() const {
  return node_index;
}

bool Node::HasParents() const {
  return !set_parent_indexes.empty();
}


int Node::GetNumParents() const {
  return set_parent_indexes.size();
}

int Node::GetNumDiscParents() const {
  return vec_disc_parent_indexes.size();
}

int Node::GetNumChildren() const {
  return set_children_indexes.size();
}

void Node::SetNodeIndex(int i) {
  if (i<0) {
    fprintf(stderr, "Error in function %s! \nInvalid index!", __FUNCTION__);
    exit(1);
  }
  node_index = i;
}

/**
 * @brief: add a discrete child node c
 * update:  "set_children_indexes"
 */
void Node::AddChild(Node *c) {
    //TODO: Reduce redundancy: combine this function with "AddChild" in ContinuousNode
    int c_idx = c->GetNodeIndex();
    set_children_indexes.insert(c_idx);
}

///**
// * @brief: add child; can be merged with AddChild in the base class.
// */
//void ContinuousNode::AddChild(Node *c) {
//    if (c->is_discrete) {
//        fprintf(stderr, "Error in function %s! \n"
//                        "Continuous node must not have discrete child!", __FUNCTION__);
//        exit(1);
//    }
//    int c_idx = c->GetNodeIndex();
//    if (set_children_indexes.find(c_idx) == set_children_indexes.end()) {
//        set_children_indexes.insert(c_idx);
//    } else {
//        fprintf(stdout, "Node #%d is already child of Node #%d", c_idx, this->GetNodeIndex());
//    }
//}

/**
 * @brief: add a generic parent node p
 */
void Node::AddParent(Node *p) {
    if (p->is_discrete) {
        AddDiscreteParent(p);
    } else {
        AddContinuousParent(p);
    }
}

/**
 * @brief: add a parent p to the node
 * update:  "set_parent_indexes",
 *          "vec_disc_parent_indexes",
 *          "set_discrete_parents_combinations"
 */
void Node::AddDiscreteParent(Node *p) {
    int p_idx = p->GetNodeIndex();//get the id of the parent p
    if (set_parent_indexes.find(p_idx) == set_parent_indexes.end()) {
        // If p is not in the parent set.
        // update "set_parent_indexes", "vec_disc_parent_indexes"
        set_parent_indexes.insert(p_idx);
        auto dp = (DiscreteNode *) p;//dp stands for "discrete parent"
        vec_disc_parent_indexes.push_back(p_idx);

        // Update possible parent configurations
        set<DiscreteConfig> new_par_combs;
        for (int i = 0; i < dp->GetDomainSize(); ++i) {
            DiscVarVal vv(p_idx, i); // get [idx of p, each value of p]
            for (auto old_par_comb : set_discrete_parents_combinations) {
                // set_discrete_parents_combinations: set< set< pair<int, int> > >
                // new_par_combs: set< set< pair<int, int> > >
                // old_par_comb: set< pair<int, int> >
                // vv: pair<int, int>
                // add each vv(corresponding to each potential value of p) to each old_par_comb
                old_par_comb.insert(vv);//insert the new parent with a potential value to the old configuration
                new_par_combs.insert(old_par_comb);
            } // finish adding one vv to all old_par_comb
        }
        this->set_discrete_parents_combinations = new_par_combs;
    }
    //if the parent is already in the set_parent_indexes, nothing needs to be done.
}

/**
 * @brief: similar to AddDiscreteParent
 * update:  "set_parent_indexes",
 *          "contin_par_indexes"
 */
void Node::AddContinuousParent(Node *p) {
  int p_idx = p->GetNodeIndex();
  if (set_parent_indexes.find(p_idx) == set_parent_indexes.end()) {  // If p is not in the parent set.
    // update "set_parent_indexes", "contin_par_indexes"
    set_parent_indexes.insert(p_idx);
    // TODO: this
    auto cont_this = (ContinuousNode*) this;
    cont_this->contin_par_indexes.push_back(p_idx);
  }
}

bool Node::IsChildOfThisNode(Node *node_ptr) {
    int node_idx = node_ptr->GetNodeIndex();
    if (set_children_indexes.find(node_idx) == set_children_indexes.end()) {
        return false;
    } else {
        return true;
    }
}

bool Node::IsParentOfThisNode(Node *node_ptr) {
    int node_idx = node_ptr->GetNodeIndex();
    if (set_parent_indexes.find(node_idx) == set_parent_indexes.end()) {
        return false;
    } else {
        return true;
    }
}

/**
 * @brief: remove a child node c; this function is used for network structure learning.
 * @attention: this function needs to use with RemoveParent
 * update:  "set_children_indexes"
 */
void Node::RemoveChild(Node *c) {
  if (!IsChildOfThisNode(c)) {
    fprintf(stderr, "Node #%d does not have parent node #%d!", this->GetNodeIndex(), c->GetNodeIndex());
    return;
  }
  int c_idx = c->GetNodeIndex();
  set_children_indexes.erase(c_idx);
}

/**
 * @brief: remove a parent node p
 * @attention: this function needs to use with RemoveChild
 * update:  "set_parent_indexes",
 *          "vec_disc_parent_indexes",
 *          "set_discrete_parents_combinations"
 *          "contin_par_indexes"
 */
void Node::RemoveParent(Node *p) {
    //TODO: combine with "RemoveParent" in ContinuousNode.cpp??
  if (!IsParentOfThisNode(p)) {
    fprintf(stderr, "Node #%d does not have parent node #%d!", this->GetNodeIndex(), p->GetNodeIndex());
    return;
  }

  int p_idx = p->GetNodeIndex();
  set_parent_indexes.erase(p_idx);

  if (p->is_discrete) {
    vec_disc_parent_indexes.erase(std::find(vec_disc_parent_indexes.begin(), vec_disc_parent_indexes.end(), p_idx));

    // Update possible parent configurations
    auto dp = (DiscreteNode*) p;
    set<DiscreteConfig> new_par_combs;
    /**
     * TODO: erase each vv(corresponding to each potential value of p) to each old_par_comb is wrong!
     * because each old_par_comb contains only one vv
     * and this will finally insert # of potential values times more combs to new_par_combs
     * just erase one config corresponding to p_idx for each old_par_comb will also insert more combs
     * (set, no repeated, so ok, but take more time)
     * maybe choose combs from set_discrete_parents_combinations that have one specific [p_idx, one of the value]
     * then erase [p_idx, one of the value] and insert to new_par_combs
     */
//    int val = 0; // just use (any) one possible value of p
//    DiscVarVal vv(p_idx, val);
//    for (auto old_par_comb : set_discrete_parents_combinations) {
//        if (old_par_comb.find(vv) != old_par_comb.end()) { // old_par_comb contains vv
//            old_par_comb.erase(vv);
//            new_par_combs.insert(old_par_comb);
//        }
//    }
      for (int i = 0; i < dp->GetDomainSize(); ++i) {
          DiscVarVal vv(p_idx, i); // get [idx of p, each value of p]
          for (auto old_par_comb : set_discrete_parents_combinations) {
              old_par_comb.erase(vv);
              new_par_combs.insert(old_par_comb);
          }
      }

    set_discrete_parents_combinations = new_par_combs;
  }
}

///**
// * @brief: remove parent; possibly only used in structure learning
// */
//void ContinuousNode::RemoveParent(Node *p) {
//    Node::RemoveParent(p);
//
//    if (!p->is_discrete) {
//        //remove the parent from the vector based on parent id
//        auto it = contin_par_indexes.begin();
//        while (*it!=p->GetNodeIndex()) { ++it; }
//        contin_par_indexes.erase(it);
//    }
//}


/**
 * @brief: possibly only used in structure learning
 * clear:   "set_parent_indexes",
 *          "vec_disc_parent_indexes",
 *          "set_discrete_parents_combinations"
 */
void Node::ClearParents() {
  //TODO: double check whether need to set "num_parents_config"
  //TODO: this function clear discrete parents but not continuous parents
  set_discrete_parents_combinations.clear();

  vec_disc_parent_indexes.clear();
  set_parent_indexes.clear();
}

void Node::ClearChildren() {
  set_children_indexes.clear();
}