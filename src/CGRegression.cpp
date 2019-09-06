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
      contin_tail_indexes.push_back(par->GetNodeIndex());
    }
  }
  set_discrete_tails_combinations = node_ptr->set_discrete_parents_combinations;
  mu = c_n_p->mu;
  coefficients = c_n_p->coefficients;
  variance = c_n_p->variance;
}

void CGRegression::Substitute(pair<int, double> var_value) {
  bool state = false;
  int coeff_index_for_var = 0;
  for (const auto &var : contin_tail_indexes) {
    if (var==var_value.first) {
      state = true;
      contin_tail_indexes.erase(contin_tail_indexes.begin()+coeff_index_for_var);
      break;
    }
    ++coeff_index_for_var;
  }
  if (!state) { exit(1); }

  for (const auto &comb : set_discrete_tails_combinations) {
    mu[comb] += var_value.second * coefficients[comb].at(coeff_index_for_var);
    coefficients[comb].erase(coefficients[comb].begin()+coeff_index_for_var);
  }
}

void CGRegression::Exchange(CGRegression &Z, CGRegression &Y) {
  PrepareForExchange(Z, Y);
  ActuallyExchange(Z, Y);
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

  if (Z.set_discrete_tails_combinations != Y.set_discrete_tails_combinations) {
    fprintf(stderr, "Error in function: %s\nThe discrete tails of "
                    "two CG regressions are not the same!", __FUNCTION__);
    exit(1);
  }

  // If y is not in Z's tail.
  if (Z.set_all_tail_index.find(Y.head_var_index) == Z.set_all_tail_index.end()) {
    Z.set_all_tail_index.insert(Y.head_var_index);
    Z.contin_tail_indexes.push_back(Y.head_var_index);
    for (const auto &comb : Z.set_discrete_tails_combinations) {
      Z.coefficients[comb].push_back(0);
    }
  }

  // If Z and Y's tails are different apart from y.
  // Then complete them to be the same.
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
    Y.contin_tail_indexes.push_back(zny);
    for (const auto &comb : Y.set_discrete_tails_combinations) {
      Y.coefficients[comb].push_back(0);
    }
  }
  for (const auto &ynz : in_Y_but_not_Z) {
    Z.set_all_tail_index.insert(ynz);
    Z.contin_tail_indexes.push_back(ynz);
    for (const auto &comb : Z.set_discrete_tails_combinations) {
      Z.coefficients[comb].push_back(0);
    }
  }
  if (0 != Z.contin_tail_indexes.size()-Y.contin_tail_indexes.size()-1) {
    fprintf(stderr, "Error in function: %s\nSomething is wrong"
                    "but I do not know how to fix it yet", __FUNCTION__);
    exit(1);
  }

  // ==================================================
  // Convert the CG regression of Z to the form the same as the paper described,
  // which is that, variable y is at the last position.
  if (Z.contin_tail_indexes.back()!=Y.head_var_index) {
    int pos_of_y = 0;
    for (const auto &idx : Z.contin_tail_indexes) {
      if (idx == Y.head_var_index) {
        break;
      }
      ++pos_of_y;
    }
    Z.contin_tail_indexes.erase(Z.contin_tail_indexes.begin() + pos_of_y,
                                Z.contin_tail_indexes.begin() + pos_of_y + 1);
    Z.contin_tail_indexes.push_back(Y.head_var_index);
    for (const auto &comb : Z.set_discrete_tails_combinations) {
      double coeff = Z.coefficients[comb].at(pos_of_y);
      Z.coefficients[comb].erase(Z.coefficients[comb].begin() + pos_of_y, Z.coefficients[comb].begin() + pos_of_y + 1);
      Z.coefficients[comb].push_back(coeff);
    }
  }

  // ==================================================
  // For implementation, the ordering of other tail variables should be the same between Z and Y.
  // Here, I adjust Z to be the same ordering as Y.
  // The loop condition is set to be continuous tail size of Y,
  // because the continuous tail size of Z is one large than Y's
  // and the last element of Z's is y.
  for (int i=0; i<Y.contin_tail_indexes.size(); ++i) {
    if (Y.contin_tail_indexes.at(i)!=Z.contin_tail_indexes.at(i)) {
      int j = i+1;
      while (j<Y.contin_tail_indexes.size() && Z.contin_tail_indexes.at(j)!=Y.contin_tail_indexes.at(i)) { ++j; }
      if (j>=Y.contin_tail_indexes.size()) { exit(1); }
      // Swap.
      int temp_var = Z.contin_tail_indexes.at(i);
      Z.contin_tail_indexes.at(i) = Z.contin_tail_indexes.at(j);
      Z.contin_tail_indexes.at(j) = temp_var;
      for (const auto &comb : Z.set_discrete_tails_combinations) {
        int temp_coeff = Z.coefficients[comb].at(i);
        Z.coefficients[comb].at(i) = Z.coefficients[comb].at(j);
        Z.coefficients[comb].at(j) = temp_coeff;
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

  if (Z.set_all_tail_index.find(Y.head_var_index) == Z.set_all_tail_index.end()) {
    fprintf(stderr, "Error in function: %s\nThe tail of the first CG regression"
                    "does not contain the head of the second CG regression!", __FUNCTION__);
    exit(1);
  }

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

  for (const auto &comb : Z.set_discrete_tails_combinations) {

    // ==================================================
    // Update Z
    double origin_mu_Z = Z.mu[comb];
    vector<double> origin_coefficients_Z = Z.coefficients[comb];
    double origin_variance_Z = Z.variance[comb];
    int b = Z.coefficients[comb].back();  // b is the coefficient for y.
    for (int i=0; i<Y.coefficients[comb].size(); ++i) {
      Z.coefficients[comb].at(i) = origin_coefficients_Z.at(i) + b * Y.coefficients[comb].at(i);
    }
    Z.variance[comb] += b*b*Y.variance[comb];

    // ==================================================
    // Update Y
    // In the paper, there are 3 cases, and the author says implementation
    // should treat these 3 cases separately. But I doubt it.
    if (Y.variance[comb]>0 && origin_variance_Z>0) {  // Case 1

      // Update mean.
      Y.mu[comb] = Y.mu[comb]*origin_variance_Z - origin_mu_Z*b*Y.variance[comb];
      for (int i=0; i<Y.coefficients[comb].size(); ++i) {
        Y.coefficients[comb].at(i) = Y.coefficients[comb].at(i)*origin_variance_Z
                                     -
                                     origin_coefficients_Z.at(i)*b*Y.variance[comb];
      }
      Y.set_all_tail_index.insert(Z.head_var_index);
      Y.contin_tail_indexes.push_back(Z.head_var_index);
      Y.coefficients[comb].push_back(b*Y.variance[comb]);
      Y.mu[comb] /= (origin_variance_Z + b*b*Y.variance[comb]);
      for (int i=0; i<Y.coefficients[comb].size(); ++i) {
        Y.coefficients[comb].at(i) /= (origin_variance_Z + b*b*Y.variance[comb]);
      }

      // Update variance.
      Y.variance[comb] = (Y.variance[comb]*origin_variance_Z) / (origin_variance_Z+b*b*Y.variance[comb]);

    } else if (Y.variance[comb]>0 && origin_variance_Z==0) {  // Case 2

      // Update mean.
      Y.mu[comb] = -origin_mu_Z;
      for (int i=0; i<Y.coefficients[comb].size(); ++i) {
        Y.coefficients[comb].at(i) = -origin_coefficients_Z.at(i);
      }
      Y.set_all_tail_index.insert(Z.head_var_index);
      Y.contin_tail_indexes.push_back(Z.head_var_index);
      Y.coefficients[comb].push_back(1);
      Y.mu[comb] /= b;
      for (int i=0; i<Y.coefficients[comb].size(); ++i) {
        Y.coefficients[comb].at(i) /= b;
      }

      // Update variance.
      Y.variance[comb] = 0;

    } else if (Y.variance[comb]==0 && origin_variance_Z>=0) {   // Case 3

      // Update mean.
      // In the paper, in this case, there is no coefficient for variable z.
      // But, since z is in the tail, there should have a coefficient, which is set to zero.
      Y.set_all_tail_index.insert(Z.head_var_index);
      Y.contin_tail_indexes.push_back(Z.head_var_index);
      Y.coefficients[comb].push_back(0);

      // Update variance.
      Y.variance[comb] = 0;

    } else {

      fprintf(stderr, "Error in function: %s\nThe variances of Z and Y do not"
                      "match any of the 3 cases described in the paper.", __FUNCTION__);
      exit(1);

    }
  }
}