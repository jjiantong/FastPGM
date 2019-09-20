//
// Created by LinjianLi on 2019/8/31.
//

#include "CGRegression.h"

CGRegression::CGRegression(Node *node_ptr) {
  if (node_ptr->is_discrete) {
    fprintf(stderr, "The node is not continuous!");
    exit(1);
  }
  ContinuousNode *c_n_p = dynamic_cast<ContinuousNode*>(node_ptr);
  head_var_index = node_ptr->GetNodeIndex();
  for (const auto &par : node_ptr->set_parents_ptrs) {
    set_all_tail_index.insert(par->GetNodeIndex());
    if (!par->is_discrete) {
      vec_contin_tail_indexes.push_back(par->GetNodeIndex());
    }
  }
  set_discrete_tails_combinations = node_ptr->set_discrete_parents_combinations;
  map_mu = c_n_p->map_mu;
  marginal_mu = c_n_p->marginal_mu;
  map_coefficients = c_n_p->map_coefficients;
  marginal_coefficients = c_n_p->marginal_coefficients;
  map_variance = c_n_p->map_variance;
  marginal_variance = c_n_p->marginal_variance;
}

string CGRegression::GetExpression() {
  set<int> temp_cont_tails, temp_disc_tails;
  temp_cont_tails.insert(vec_contin_tail_indexes.begin(), vec_contin_tail_indexes.end());
  set_difference(set_all_tail_index.begin(), set_all_tail_index.end(),
                 temp_cont_tails.begin(), temp_cont_tails.end(),
                 inserter(temp_disc_tails, temp_disc_tails.begin()));
  string expression = "";
  expression += "L(" + to_string(head_var_index) + "|";
  for (const auto &v : temp_cont_tails) {
    expression += to_string(v) + ",";
  }
  expression = expression.substr(0, expression.size()-1) + ";";
  for (const auto &v : temp_disc_tails) {
    expression += to_string(v) + ",";
  }
  expression = expression.substr(0, expression.size()-1) + ")";
  return expression;
}

void CGRegression::Substitute(pair<int, double> var_value) {
  bool state = false;
  int coeff_index_for_var = 0;
  for (const auto &var : vec_contin_tail_indexes) {
    if (var==var_value.first) {
      state = true;
      break;
    }
    ++coeff_index_for_var;
  }
  if (!state) {
    fprintf(stderr, "Error in function: %s\nCG regression %s does not "
                    "include continuous variable [%d]", __FUNCTION__, this->GetExpression().c_str(), var_value.first);
    exit(1);
  }

  set_all_tail_index.erase(set_all_tail_index.find(var_value.first));
  vec_contin_tail_indexes.erase(vec_contin_tail_indexes.begin() + coeff_index_for_var);
  if (!set_discrete_tails_combinations.empty()) {
    for (const auto &comb : set_discrete_tails_combinations) {
      map_mu[comb] += var_value.second * map_coefficients[comb].at(coeff_index_for_var);
      map_coefficients[comb].erase(map_coefficients[comb].begin() + coeff_index_for_var);
      if (map_coefficients[comb].size() != vec_contin_tail_indexes.size()) {
        fprintf(stderr, "Error in function: %s", __FUNCTION__);
        exit(1);
      }
    }
  } else {
    marginal_mu += var_value.second * marginal_coefficients.at(coeff_index_for_var);
    marginal_coefficients.erase(marginal_coefficients.begin() + coeff_index_for_var);
  }
}

void CGRegression::Exchange(CGRegression &Z, CGRegression &Y) {
  cout << "==========\nExchanging:\n"
       << Z.GetExpression() << '\t'
       << Y.GetExpression() << endl;

  PrepareForExchange(Z, Y);

  cout << "After preparing for exchanging:\n"
       << Z.GetExpression() << '\t'
       << Y.GetExpression() << endl;

  ActuallyExchange(Z, Y);

  cout << "After exchanging:\n"
       << Z.GetExpression() << '\t'
       << Y.GetExpression() << endl;
  cout << "==========" << endl;
}

void CGRegression::PrepareForExchange(CGRegression &Z, CGRegression &Y) {
  // todo: test correctness
  // The Exchange operation described by Cowell requires that
  // apart from variable y, Z and Y should contain the same tail variables W_1 to W_l.
  // Sometimes, this may not be true.
  // In implementation, we can just complete the W to be the same for
  // Z and Y, and set certain coefficients to zero as needed.
  // For example, if Y does not depend on W_2, then we can set c_2 to zero.
  // Also, if Z's tail does not contain y, we can set coefficient, b, for y to zero.

  bool z_not_have_disc_parents = Z.set_discrete_tails_combinations.empty();
  bool y_not_have_disc_parents = Y.set_discrete_tails_combinations.empty();

  // Deal with the case where discrete tails are different.
  if (Z.set_discrete_tails_combinations != Y.set_discrete_tails_combinations) {

    cout << "The discrete tails of two CG regressions are not the same! Need expanding." << endl;

    cout << Z.GetExpression() << '\t'
         << Y.GetExpression() << endl;

    set<DiscreteConfig> union_two_cgr_disc_combs
            = ExpandConfgFromTwoConfgs(&Z.set_discrete_tails_combinations, &Y.set_discrete_tails_combinations);

    Z.set_discrete_tails_combinations = union_two_cgr_disc_combs;
    Y.set_discrete_tails_combinations = union_two_cgr_disc_combs;

    // Add discrete tails to be the same.

    set<int> temp_cont_tails, temp_disc_tails;

    // Add discrete tails to Z
    temp_cont_tails.insert(Y.vec_contin_tail_indexes.begin(), Y.vec_contin_tail_indexes.end());
    set_difference(Y.set_all_tail_index.begin(), Y.set_all_tail_index.end(),
                   temp_cont_tails.begin(), temp_cont_tails.end(),
                   inserter(temp_disc_tails, temp_disc_tails.begin()));
    Z.set_all_tail_index.insert(temp_disc_tails.begin(), temp_disc_tails.end());

    // Add discrete tails to Y
    temp_cont_tails.clear();
    temp_disc_tails.clear();
    temp_cont_tails.insert(Z.vec_contin_tail_indexes.begin(), Z.vec_contin_tail_indexes.end());
    set_difference(Z.set_all_tail_index.begin(), Z.set_all_tail_index.end(),
                   temp_cont_tails.begin(), temp_cont_tails.end(),
                   inserter(temp_disc_tails, temp_disc_tails.begin()));
    Y.set_all_tail_index.insert(temp_disc_tails.begin(), temp_disc_tails.end());

    // Expand Z
    map<DiscreteConfig, double> Z_mu_expand;
    map<DiscreteConfig, vector<double>> Z_coefficients_expand;
    map<DiscreteConfig, double> Z_variance_expand;
    if (!z_not_have_disc_parents) {
      for (const auto &kv : Z.map_mu) {
        for (const auto &comb : union_two_cgr_disc_combs) {
          if (!Conflict(&kv.first, &comb)) {
            Z_mu_expand[comb] = Z.map_mu[kv.first];
            Z_coefficients_expand[comb] = Z.map_coefficients[kv.first];
            Z_variance_expand[comb] = Z.map_variance[kv.first];
          }
        }
      }
    } else {
      for (const auto &comb : union_two_cgr_disc_combs) {
        Z_mu_expand[comb] = Z.marginal_mu;
        Z_coefficients_expand[comb] = Z.marginal_coefficients;
        Z_variance_expand[comb] = Z.marginal_variance;
      }
    }
    Z.map_mu = Z_mu_expand;
    Z.map_coefficients = Z_coefficients_expand;
    Z.map_variance = Z_variance_expand;

    // Expand Y
    map<DiscreteConfig, double> Y_mu_expand;
    map<DiscreteConfig, vector<double>> Y_coefficients_expand;
    map<DiscreteConfig, double> Y_variance_expand;
    if (!y_not_have_disc_parents) {
      for (const auto &kv : Y.map_mu) {
        for (const auto &comb : union_two_cgr_disc_combs) {
          if (!Conflict(&kv.first, &comb)) {
            Y_mu_expand[comb] = Y.map_mu[kv.first];
            Y_coefficients_expand[comb] = Y.map_coefficients[kv.first];
            Y_variance_expand[comb] = Y.map_variance[kv.first];
          }
        }
      }
    } else {
      for (const auto &comb : union_two_cgr_disc_combs) {
        Y_mu_expand[comb] = Y.marginal_mu;
        Y_coefficients_expand[comb] = Y.marginal_coefficients;
        Y_variance_expand[comb] = Y.marginal_variance;
      }
    }
    Y.map_mu = Y_mu_expand;
    Y.map_coefficients = Y_coefficients_expand;
    Y.map_variance = Y_variance_expand;

    cout << "After expanding the discrete tails:" << endl;

    cout << Z.GetExpression() << '\t'
         << Y.GetExpression() << endl;

  }   // end of: if (Z.set_discrete_tails_combinations != Y.set_discrete_tails_combinations)


  // If y is not in Z's tail, add it in and set coefficient to zero.
  if (Z.set_all_tail_index.find(Y.head_var_index) == Z.set_all_tail_index.end()) {
    Z.set_all_tail_index.insert(Y.head_var_index);
    Z.vec_contin_tail_indexes.push_back(Y.head_var_index);
    if (!z_not_have_disc_parents || !y_not_have_disc_parents) {
      for (const auto &comb : Z.set_discrete_tails_combinations) {
        Z.map_coefficients[comb].push_back(0);
      }
    } else {
      Z.marginal_coefficients.push_back(0);
    }
  }

  // If Z and Y's continuous tails are different apart from y.
  // Then complete them to be the same, and set corresponding coefficients to zero.
  set<int> in_Z_but_not_Y, in_Y_but_not_Z;
  set_difference(Z.set_all_tail_index.begin(), Z.set_all_tail_index.end(),
                 Y.set_all_tail_index.begin(), Y.set_all_tail_index.end(),
                 inserter(in_Z_but_not_Y, in_Z_but_not_Y.begin()));
  set_difference(Y.set_all_tail_index.begin(), Y.set_all_tail_index.end(),
                 Z.set_all_tail_index.begin(), Z.set_all_tail_index.end(),
                 inserter(in_Y_but_not_Z, in_Y_but_not_Z.begin()));
  for (const auto &zny : in_Z_but_not_Y) {
    if (zny==Y.head_var_index) { continue; }  // Y should not depend on y itself.
    Y.set_all_tail_index.insert(zny);
    Y.vec_contin_tail_indexes.push_back(zny);
    if (!z_not_have_disc_parents || !y_not_have_disc_parents) {
      for (const auto &comb : Y.set_discrete_tails_combinations) {
        Y.map_coefficients[comb].push_back(0);
      }
    } else {
      Y.marginal_coefficients.push_back(0);
    }
  }
  for (const auto &ynz : in_Y_but_not_Z) {
    Z.set_all_tail_index.insert(ynz);
    Z.vec_contin_tail_indexes.push_back(ynz);
    if (!z_not_have_disc_parents || !y_not_have_disc_parents) {
      for (const auto &comb : Z.set_discrete_tails_combinations) {
        Z.map_coefficients[comb].push_back(0);
      }
    } else {
      Z.marginal_coefficients.push_back(0);
    }
  }
  if (0 != Z.vec_contin_tail_indexes.size() - Y.vec_contin_tail_indexes.size() - 1) {
    fprintf(stderr, "Error in function: %s\nSomething is wrong "
                    "but I do not know how to fix it yet", __FUNCTION__);
    exit(1);
  }

  // ==================================================
  // Convert the CG regression of Z to the form the same as the paper described,
  // which is that, variable y is at the last position of continuous tails.
  if (Z.vec_contin_tail_indexes.back() != Y.head_var_index) {
    int pos_of_y = 0;
    for (const auto &idx : Z.vec_contin_tail_indexes) {
      if (idx == Y.head_var_index) {
        break;
      }
      ++pos_of_y;
    }
    Z.vec_contin_tail_indexes.erase(Z.vec_contin_tail_indexes.begin() + pos_of_y,
                                    Z.vec_contin_tail_indexes.begin() + pos_of_y + 1);
    Z.vec_contin_tail_indexes.push_back(Y.head_var_index);
    if (!z_not_have_disc_parents || !y_not_have_disc_parents) {
      for (const auto &comb : Z.set_discrete_tails_combinations) {
        double coeff = Z.map_coefficients[comb].at(pos_of_y);
        Z.map_coefficients[comb].erase(Z.map_coefficients[comb].begin() + pos_of_y,
                                       Z.map_coefficients[comb].begin() + pos_of_y + 1);
        Z.map_coefficients[comb].push_back(coeff);
      }
    } else {
      double coeff = Z.marginal_coefficients.at(pos_of_y);
      Z.marginal_coefficients.erase(Z.marginal_coefficients.begin() + pos_of_y,
                                     Z.marginal_coefficients.begin() + pos_of_y + 1);
      Z.marginal_coefficients.push_back(coeff);
    }
  }

  // ==================================================
  // For implementation, the ordering of other continuous tail variables should be the same between Z and Y.
  // Here, I adjust Z to be the same ordering as Y.
  // The loop condition is set to be continuous tail size of Y,
  // because the continuous tail size of Z is one large than Y's
  // and the last element of Z's is y.
  for (int i=0; i<Y.vec_contin_tail_indexes.size(); ++i) {
    if (Y.vec_contin_tail_indexes.at(i) != Z.vec_contin_tail_indexes.at(i)) {
      int j = i+1;
      while (j<Y.vec_contin_tail_indexes.size() && Z.vec_contin_tail_indexes.at(j) != Y.vec_contin_tail_indexes.at(i)) { ++j; }
      if (j>=Y.vec_contin_tail_indexes.size()) { exit(1); }
      // Swap.
      int temp_var = Z.vec_contin_tail_indexes.at(i);
      Z.vec_contin_tail_indexes.at(i) = Z.vec_contin_tail_indexes.at(j);
      Z.vec_contin_tail_indexes.at(j) = temp_var;
      if (!z_not_have_disc_parents || !y_not_have_disc_parents) {
        for (const auto &comb : Z.set_discrete_tails_combinations) {
          int temp_coeff = Z.map_coefficients[comb].at(i);
          Z.map_coefficients[comb].at(i) = Z.map_coefficients[comb].at(j);
          Z.map_coefficients[comb].at(j) = temp_coeff;
        }
      } else {
        int temp_coeff = Z.marginal_coefficients.at(i);
        Z.marginal_coefficients.at(i) = Z.marginal_coefficients.at(j);
        Z.marginal_coefficients.at(j) = temp_coeff;
      }
    }
  }
}

void CGRegression::ActuallyExchange(CGRegression &Z, CGRegression &Y) {
  // todo: test correctness
  // Described in [Local Propagation in Conditional Gaussian Bayesian Networks (Cowell, 2005)].
  // Section 5.3. Page 16.
  // CGRegression Z should contains variable y in tail.
  // And apart from variable y, Z and Y should contain the same tail variables.
  // For example, Z=L(z|y,a,b,c,d,blah) and Y=L(y|a,b,c,d,blah).
  // Also, for implementation, the ordering of other tail variables should be the same between Z and Y.
  // The requirements above will be satisfied by the function of PrepareForExchange(CGRegression &Z, CGRegression &Y).

  if (Z.set_all_tail_index.find(Y.head_var_index) == Z.set_all_tail_index.end()) {
    fprintf(stderr, "Error in function: %s\nThe tail of the first CG regression "
                    "does not contain the head of the second CG regression!", __FUNCTION__);
    exit(1);
  }

  bool use_marginal = (Z.set_discrete_tails_combinations.empty() && Y.set_discrete_tails_combinations.empty());

  set<int> diff;
  set_difference(Z.set_all_tail_index.begin(), Z.set_all_tail_index.end(),
                 Y.set_all_tail_index.begin(), Y.set_all_tail_index.end(),
                 inserter(diff, diff.begin()));
  diff.erase(Y.head_var_index);
  if (!diff.empty()
      &&
      Z.set_all_tail_index.size()-Y.set_all_tail_index.size()-1 != 0) {
    fprintf(stderr, "Error in function: %s\nApart from variable y, Z and Y should contain the same"
                    "tail variables. The arguments passed in do not satisfy this condition.", __FUNCTION__);
    exit(1);
  }

  if (!use_marginal) {
    for (const auto &comb : Z.set_discrete_tails_combinations) {

      // ==================================================
      // Update Z
      double origin_mu_Z = Z.map_mu[comb];
      vector<double> origin_coefficients_Z = Z.map_coefficients[comb];
      double origin_variance_Z = Z.map_variance[comb];
      int b = Z.map_coefficients[comb].back();  // b is the coefficient for y.
      Z.map_mu[comb] = origin_mu_Z + b * Y.map_mu[comb];
      Z.vec_contin_tail_indexes.erase(Z.vec_contin_tail_indexes.end() - 1); // Remove the dependency on y.
      Z.map_coefficients[comb].erase(Z.map_coefficients[comb].end() - 1);   // Remove the dependency on y.
      Z.set_all_tail_index.erase(Y.head_var_index);               // Remove the dependency on y.
      for (int i = 0; i < Y.map_coefficients[comb].size(); ++i) {
        Z.map_coefficients[comb].at(i) = origin_coefficients_Z.at(i) + b * Y.map_coefficients[comb].at(i);
      }
      Z.map_variance[comb] += b * b * Y.map_variance[comb];

      // ==================================================
      // Update Y
      // In the paper, there are 3 cases, and the author says implementation
      // should treat these 3 cases separately. But I doubt it.
      if (Y.map_variance[comb] > 0 && origin_variance_Z > 0) {  // Case 1

        // Update mean.
        Y.map_mu[comb] = Y.map_mu[comb] * origin_variance_Z - origin_mu_Z * b * Y.map_variance[comb];
        for (int i = 0; i < Y.map_coefficients[comb].size(); ++i) {
          Y.map_coefficients[comb].at(i) = Y.map_coefficients[comb].at(i) * origin_variance_Z
                                           -
                                           origin_coefficients_Z.at(i) * b * Y.map_variance[comb];
        }
        Y.set_all_tail_index.insert(Z.head_var_index);
        Y.vec_contin_tail_indexes.push_back(Z.head_var_index);
        Y.map_coefficients[comb].push_back(b * Y.map_variance[comb]);
        Y.map_mu[comb] /= (origin_variance_Z + b * b * Y.map_variance[comb]);
        for (int i = 0; i < Y.map_coefficients[comb].size(); ++i) {
          Y.map_coefficients[comb].at(i) /= (origin_variance_Z + b * b * Y.map_variance[comb]);
        }

        // Update variance.
        Y.map_variance[comb] =
                (Y.map_variance[comb] * origin_variance_Z) / (origin_variance_Z + b * b * Y.map_variance[comb]);

      } else if (Y.map_variance[comb] > 0 && origin_variance_Z == 0) {  // Case 2

        // Update mean.
        Y.map_mu[comb] = -origin_mu_Z;
        for (int i = 0; i < Y.map_coefficients[comb].size(); ++i) {
          Y.map_coefficients[comb].at(i) = -origin_coefficients_Z.at(i);
        }
        Y.set_all_tail_index.insert(Z.head_var_index);
        Y.vec_contin_tail_indexes.push_back(Z.head_var_index);
        Y.map_coefficients[comb].push_back(1);
        Y.map_mu[comb] /= b;
        for (int i = 0; i < Y.map_coefficients[comb].size(); ++i) {
          Y.map_coefficients[comb].at(i) /= b;
        }

        // Update variance.
        Y.map_variance[comb] = 0;

      } else if (Y.map_variance[comb] == 0 && origin_variance_Z >= 0) {   // Case 3

        // Update mean.
        // In the paper, in this case, there is no coefficient for variable z.
        // But, since z is in the tail, there should have a coefficient, which is set to zero.
        Y.set_all_tail_index.insert(Z.head_var_index);
        Y.vec_contin_tail_indexes.push_back(Z.head_var_index);
        Y.map_coefficients[comb].push_back(0);

        // Update variance.
        Y.map_variance[comb] = 0;

      } else {

        fprintf(stderr, "Error in function: %s\nThe variances of Z and Y do not "
                        "match any of the 3 cases described in the paper.", __FUNCTION__);
        exit(1);

      }
    }
  } else {  // if use marignal
    // ==================================================
    // Update Z
    double origin_mu_Z = Z.marginal_mu;
    vector<double> origin_coefficients_Z = Z.marginal_coefficients;
    double origin_variance_Z = Z.marginal_variance;
    int b = Z.marginal_coefficients.back();  // b is the coefficient for y.
    Z.marginal_mu = origin_mu_Z + b * Y.marginal_mu;
    Z.vec_contin_tail_indexes.erase(Z.vec_contin_tail_indexes.end() - 1); // Remove the dependency on y.
    Z.marginal_coefficients.erase(Z.marginal_coefficients.end() - 1);   // Remove the dependency on y.
    Z.set_all_tail_index.erase(Y.head_var_index);               // Remove the dependency on y.
    for (int i = 0; i < Y.marginal_coefficients.size(); ++i) {
      Z.marginal_coefficients.at(i) = origin_coefficients_Z.at(i) + b * Y.marginal_coefficients.at(i);
    }
    Z.marginal_variance += b * b * Y.marginal_variance;

    // ==================================================
    // Update Y
    // In the paper, there are 3 cases, and the author says implementation
    // should treat these 3 cases separately. But I doubt it.
    if (Y.marginal_variance > 0 && origin_variance_Z > 0) {  // Case 1

      // Update mean.
      Y.marginal_mu = Y.marginal_mu * origin_variance_Z - origin_mu_Z * b * Y.marginal_variance;
      for (int i = 0; i < Y.marginal_coefficients.size(); ++i) {
        Y.marginal_coefficients.at(i) = Y.marginal_coefficients.at(i) * origin_variance_Z
                                        -
                                        origin_coefficients_Z.at(i) * b * Y.marginal_variance;
      }
      Y.set_all_tail_index.insert(Z.head_var_index);
      Y.vec_contin_tail_indexes.push_back(Z.head_var_index);
      Y.marginal_coefficients.push_back(b * Y.marginal_variance);
      Y.marginal_mu /= (origin_variance_Z + b * b * Y.marginal_variance);
      for (int i = 0; i < Y.marginal_coefficients.size(); ++i) {
        Y.marginal_coefficients.at(i) /= (origin_variance_Z + b * b * Y.marginal_variance);
      }

      // Update variance.
      Y.marginal_variance =
              (Y.marginal_variance * origin_variance_Z) / (origin_variance_Z + b * b * Y.marginal_variance);

    } else if (Y.marginal_variance > 0 && origin_variance_Z == 0) {  // Case 2

      // Update mean.
      Y.marginal_mu = -origin_mu_Z;
      for (int i = 0; i < Y.marginal_coefficients.size(); ++i) {
        Y.marginal_coefficients.at(i) = -origin_coefficients_Z.at(i);
      }
      Y.set_all_tail_index.insert(Z.head_var_index);
      Y.vec_contin_tail_indexes.push_back(Z.head_var_index);
      Y.marginal_coefficients.push_back(1);
      Y.marginal_mu /= b;
      for (int i = 0; i < Y.marginal_coefficients.size(); ++i) {
        Y.marginal_coefficients.at(i) /= b;
      }

      // Update variance.
      Y.marginal_variance = 0;

    } else if (Y.marginal_variance == 0 && origin_variance_Z >= 0) {   // Case 3

      // Update mean.
      // In the paper, in this case, there is no coefficient for variable z.
      // But, since z is in the tail, there should have a coefficient, which is set to zero.
      Y.set_all_tail_index.insert(Z.head_var_index);
      Y.vec_contin_tail_indexes.push_back(Z.head_var_index);
      Y.marginal_coefficients.push_back(0);

      // Update variance.
      Y.marginal_variance = 0;

    } else {

      fprintf(stderr, "Error in function: %s\nThe variances of Z and Y do not "
                      "match any of the 3 cases described in the paper.", __FUNCTION__);
      exit(1);

    }
  }
}