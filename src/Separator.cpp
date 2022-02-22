//
// Created by LinjianLi on 2019/2/20.
//

#include "Separator.h"

Separator::Separator() {
  is_separator = true;
  weight = -1;
//    old_table = new Factor();
}

Separator::Separator(set<Node*> set_node_ptr): Clique(set_node_ptr) {
  is_separator = true;
  weight = clique_size;
//  old_table = new Factor();
}

//Separator::~Separator(){
//    delete old_table;
//}

Separator* Separator::CopyWithoutPtr() {
  auto s = new Separator(*this);
  s->set_neighbours_ptr.clear();
  s->ptr_upstream_clique = nullptr;
  return s;
}


void Separator::UpdateUseMessage(Factor &f, Timer *timer) {
//    timer->Start("update sep");
    old_table = table;
    SumOutExternalVars(f, timer);
    table = f;
//    timer->Stop("update sep");
}

/**
 * @brief: this is a standard process for constructing the message of the separator cliques.
 */
void Separator::ConstructMessage(Timer *timer) {
//    timer->Start("construct sep");
//    timer->Start("factor division");
//    if (table.related_variables.size() != old_table.related_variables.size()) {
//        cout << "error!!!!!!" << endl;
//        // TODO: exit
//    }

    // if related variable of both new and old are empty
    if (table.related_variables.empty()) {
        // do nothing, just return, because "table" is a constant
        return;
    }

//    double *t1 = new double[table.set_disc_configs.size()]();
//    double *t2 = new double[table.set_disc_configs.size()]();
//    int i = 0;
//    for (auto &comb : table.set_disc_configs) {
//        t1[i] = table.map_potentials[comb];
//        t2[i] = old_table.map_potentials[comb];
//        i++;
//    }
//
////#pragma omp parallel for
//    for (int j = 0; j < table.set_disc_configs.size(); ++j) {
//        if (t2[j] == 0) {
//            t1[j] = 0;
//        } else {
//            t1[j] /= t2[j];
//        }
//    }
//
//    i = 0;
//    for (auto &comb : table.set_disc_configs) {
//        table.map_potentials[comb] = t1[i++];
//    }
//
//    delete t1;
//    delete t2;

    for (auto &comb : table.set_disc_configs) {
        if (old_table.map_potentials[comb] == 0) {
            table.map_potentials[comb] = 0;
        } else {
            table.map_potentials[comb] /= old_table.map_potentials[comb];
        }
    }
//    timer->Stop("factor division");
//    timer->Stop("construct sep");
}