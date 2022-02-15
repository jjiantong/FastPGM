//
// Created by LinjianLi on 2019/2/20.
//

#include "Separator.h"

Separator::Separator() {
  is_separator = true;
  weight = -1;
}

Separator::Separator(set<Node*> set_node_ptr): Clique(set_node_ptr) {
  is_separator = true;
  weight = clique_size;
}


Separator* Separator::CopyWithoutPtr() {
  auto s = new Separator(*this);
  s->set_neighbours_ptr.clear();
  s->ptr_upstream_clique = nullptr;
  return s;
}


void Separator::UpdateUseMessage(Factor f) {

//    cout << "update msg of sep ";
//    for (auto &v: clique_variables) {
//        cout << v << " ";
//    }
//    cout << " ( use factor ";
//    for (auto &v: f.related_variables) {
//        cout << v << " ";
//    }
//    cout << "): " << endl;

  f = SumOutExternalVars(f);

//    cout << "  save old: ";
//    for (auto &v: related_variables) {
//        cout << v << " ";
//    }
//    cout << ": " << endl;
//    for (auto &c: set_disc_configs) {
//        cout << "    ";
//        for (auto &p: c) { //pair<int, int>
//            cout << p.first << "=" << p.second << " ";
//        }
//        cout << ": " << map_potentials[c] << endl;
//    }
//    cout << endl;
//
//    cout << "  save new: ";
//    for (auto &v: f.related_variables) {
//        cout << v << " ";
//    }
//    cout << ": " << endl;
//    for (auto &c: f.set_disc_configs) {
//        cout << "    ";
//        for (auto &p: c) { //pair<int, int>
//            cout << p.first << "=" << p.second << " ";
//        }
//        cout << ": " << f.map_potentials[c] << endl;
//    }
//    cout << endl;

    old_related_variables = related_variables;
    set_old_disc_configs = set_disc_configs;
  map_old_potentials = map_potentials;
    related_variables = f.related_variables;
    set_disc_configs = f.set_disc_configs;
  map_potentials = f.map_potentials;




}

/**
 * @brief: this is a standard process for constructing the message of the separator cliques.
 */
Factor Separator::ConstructMessage() {

//    cout << "construct msg of sep ";
//    for (auto &v: clique_variables) {
//        cout << v << " ";
//    }
//    cout << ": " << endl;

    Factor f(related_variables, set_disc_configs, map_potentials);

//    if (f.map_potentials.size() != map_old_potentials.size()) {
//        cout << "error!! old = " << map_old_potentials.size() << ", new = " << f.map_potentials.size() << endl;
//    }

//    cout << "  division ";
//    for (auto &v: f.related_variables) {
//        cout << v << " ";
//    }
//    cout << "and ";
//    for (auto &v: old_related_variables) {
//        cout << v << " ";
//    }
//    cout << endl;
//
//    cout << "    before: ";
//    for (auto &v: f.related_variables) {
//        cout << v << " ";
//    }
//    cout << ": " << endl;
//    for (auto &c: f.set_disc_configs) {
//        cout << "      ";
//        for (auto &p: c) { //pair<int, int>
//            cout << p.first << "=" << p.second << " ";
//        }
//        cout << ": " << f.map_potentials[c] << endl;
//    }
//    cout << endl;
//
//    cout << "    before: ";
//    for (auto &v: old_related_variables) {
//        cout << v << " ";
//    }
//    cout << ": " << endl;
//    for (auto &c: set_old_disc_configs) {
//        cout << "      ";
//        for (auto &p: c) { //pair<int, int>
//            cout << p.first << "=" << p.second << " ";
//        }
//        cout << ": " << map_old_potentials[c] << endl;
//    }
//    cout << endl;


    if (f.related_variables.size() != old_related_variables.size()) {
        cout << "error!!!!!!" << endl;
        // TODO: exit
    }

    // case 1: related variable of both new and old are empty
    if (f.related_variables.empty()) {
        // do nothing, just return f, a constant
        return f;
    }

    for (auto &comb : f.set_disc_configs) {
        if (map_old_potentials[comb] == 0) {
            f.map_potentials[comb] = 0;
        } else {
            f.map_potentials[comb] /= map_old_potentials[comb];
        }
    }

//    cout << "    after: ";
//    for (auto &v: f.related_variables) {
//        cout << v << " ";
//    }
//    cout << ": " << endl;
//    for (auto &c: f.set_disc_configs) {
//        cout << "      ";
//        for (auto &p: c) { //pair<int, int>
//            cout << p.first << "=" << p.second << " ";
//        }
//        cout << ": " << f.map_potentials[c] << endl;
//    }
//    cout << endl;


    return f;
}