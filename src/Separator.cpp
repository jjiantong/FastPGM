//
// Created by LinjianLi on 2019/2/20.
//

#include "Separator.h"

Separator::Separator() {
  is_separator = true;
  weight = -1;
    old_table = new Factor();
}

Separator::Separator(set<Node*> set_node_ptr): Clique(set_node_ptr) {
  is_separator = true;
  weight = clique_size;
  old_table = new Factor();
}

Separator::~Separator(){
    delete old_table;
}

Separator* Separator::CopyWithoutPtr() {
  auto s = new Separator(*this);
  s->set_neighbours_ptr.clear();
  s->ptr_upstream_clique = nullptr;
  return s;
}


void Separator::UpdateUseMessage(Factor f, Timer *timer) {

//    cout << "update msg of sep ";
//    for (auto &v: clique_variables) {
//        cout << v << " ";
//    }
//    cout << " ( use factor ";
//    for (auto &v: f.related_variables) {
//        cout << v << " ";
//    }
//    cout << "): " << endl;

//    timer->Start("update sep");
  f = SumOutExternalVars(f, timer);

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

//    timer->Start("copy 3");
    old_table->related_variables = related_variables;
    old_table->set_disc_configs = set_disc_configs;
    old_table->map_potentials = map_potentials;
    related_variables = f.related_variables;
    set_disc_configs = f.set_disc_configs;
  map_potentials = f.map_potentials;
//    timer->Stop("copy 3");
//    timer->Stop("update sep");
}

/**
 * @brief: this is a standard process for constructing the message of the separator cliques.
 */
Factor Separator::ConstructMessage(Timer *timer) {

//    cout << "construct msg of sep ";
//    for (auto &v: clique_variables) {
//        cout << v << " ";
//    }
//    cout << ": " << endl;

//    timer->Start("construct sep");
//    timer->Start("construct factor");
    Factor f(related_variables, set_disc_configs, map_potentials);
//    timer->Stop("construct factor");

//    if (f.map_potentials.size() != old_table->map_potentials.size()) {
//        cout << "error!! old = " << old_table->map_potentials.size() << ", new = " << f.map_potentials.size() << endl;
//    }

//    cout << "  division ";
//    for (auto &v: f.related_variables) {
//        cout << v << " ";
//    }
//    cout << "and ";
//    for (auto &v: old_table->related_variables) {
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
//    for (auto &v: old_table->related_variables) {
//        cout << v << " ";
//    }
//    cout << ": " << endl;
//    for (auto &c: old_table->set_disc_configs) {
//        cout << "      ";
//        for (auto &p: c) { //pair<int, int>
//            cout << p.first << "=" << p.second << " ";
//        }
//        cout << ": " << old_table->map_potentials[c] << endl;
//    }
//    cout << endl;

//    timer->Start("factor division");
    if (f.related_variables.size() != old_table->related_variables.size()) {
        cout << "error!!!!!!" << endl;
        // TODO: exit
    }

    // case 1: related variable of both new and old are empty
    if (f.related_variables.empty()) {
        // do nothing, just return f, a constant
        return f;
    }

//    double *t1 = new double[f.set_disc_configs.size()]();
//    double *t2 = new double[f.set_disc_configs.size()]();
//    double *t3 = new double[f.set_disc_configs.size()]();
//    int i = 0;
//    for (auto &comb : f.set_disc_configs) {
//        t1[i] = f.map_potentials[comb];
//        t2[i] = old_table->map_potentials[comb];
//    }
//
////#pragma omp parallel for
//    for (int j = 0; j < f.set_disc_configs.size(); ++j) {
//        t3[j] = t1[j] / t2[j];
//    }
//
//    i = 0;
//    for (auto &comb : f.set_disc_configs) {
//        f.map_potentials[comb] = t3[i];
//    }
//
//    delete t1;
//    delete t2;
//    delete t3;

    for (auto &comb : f.set_disc_configs) {
        if (old_table->map_potentials[comb] == 0) {
            f.map_potentials[comb] = 0;
        } else {
            f.map_potentials[comb] /= old_table->map_potentials[comb];
        }
    }
//    timer->Stop("factor division");

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
//    timer->Stop("construct sep");

    return f;
}