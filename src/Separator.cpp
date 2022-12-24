//#include "Separator.h"
//
//Separator::Separator() {
//    is_in_jt = false;
//}
//
//Separator::Separator(set<int> set_node_index, Network *net): Clique(set_node_index, net) {
//    is_in_jt = false;
//}
//
///**
// * merge the above two methods
// */
//void Separator::UpdateMessage(const PotentialTable &pt) {
//    old_ptable = p_table;
//    PotentialTable tmp_pt = pt;
//
//    if (pt.num_variables - this->old_ptable.num_variables != 0) {
//        // need to do the marginalization
//        tmp_pt.TableMarginalization(this->old_ptable.vec_related_variables, this->old_ptable.var_dims);
//    }
//
//    if (old_ptable.vec_related_variables != pt.vec_related_variables) {
//        // if two tables have the same size but different orderings
//        // change the order to this table's order
//        tmp_pt.TableReorganization(old_ptable);
//    }
//
//    tmp_pt.TableDivision(old_ptable);
//    p_table = tmp_pt;
//}