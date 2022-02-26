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

void Separator::UpdateUseMessage2(PotentialTable &pt, Timer *timer) {
//    timer->Start("update sep");
//    cout << "construct msg of sep ";
//    for (auto &v: this->p_table.related_variables) {
//        cout << v << " ";
//    }
//    cout << " use msg ";
//    for (auto &v: pt.related_variables) {
//        cout << v << " ";
//    }
//    cout << endl;
//    cout << "tables: " << endl;
//    for (int i = 0; i < this->p_table.table_size; ++i) {
//        cout << this->p_table.potentials[i] << " ";
//    }
//    cout << endl;
//    for (int i = 0; i < pt.table_size; ++i) {
//        cout << pt.potentials[i] << " ";
//    }
//    cout << endl;

    old_ptable = p_table;
    SumOutExternalVars(pt, timer);
    p_table = pt;
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

void Separator::ConstructMessage2(Timer *timer) {
//    timer->Start("construct sep");
//    timer->Start("factor division");
//    if (table.related_variables.size() != old_table.related_variables.size()) {
//        cout << "error!!!!!!" << endl;
//        // TODO: exit
//    }

//    cout << "construct msg of sep ";
//    for (auto &v: this->p_table.related_variables) {
//        cout << v << " ";
//    }
//    cout << endl << "table: ";
//    for (int i = 0; i < this->p_table.table_size; ++i) {
//        cout << this->p_table.potentials[i] << " ";
//    }
//    cout << endl << "old ones: ";
//    for (int i = 0; i < old_ptable.table_size; ++i) {
//        cout << old_ptable.potentials[i] << " ";
//    }
//    cout << endl;

    // if related variable of both new and old are empty
    if (p_table.related_variables.empty()) {
        // do nothing, just return, because "p_table" is a constant
        return;
    }

    for (int i = 0; i < p_table.table_size; ++i) {
        if (old_ptable.potentials[i] == 0) {
            p_table.potentials[i] = 0;
        } else {
            p_table.potentials[i] /= old_ptable.potentials[i];
        }
    }
//    timer->Stop("factor division");
//    timer->Stop("construct sep");
}