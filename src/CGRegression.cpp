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

void CGRegression::Exchange(CGRegression Z, CGRegression Y) {
  // Described in [Local Propagation in Conditional Gaussian Bayesian Networks (Cowell, 2005)].
  // Section 5.3. Page 16.
  // CGRegression Z should contains variable y in tail.
  // And apart from variable y, Z and Y should contain the same tail variables.
  // For example, Z=L(z|y,a,b,c,d,e,blahblah) and Y=L(y|a,b,c,d,e,blahblah).
  // Also, for implementation, the ordering of other tail variables should be the same between Z and Y.

  // Here, tails of Z and Y contain the same W_1 to W_l. Sometimes, this may not
  // be true. In implementation, we can just complete the W to be the same for
  // Z and Y, and set certain coefficients to zero as needed.
  // For example, if Y does not depend on W_2, then we can set c_2 to zero.
  // todo: deal with this situation

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

  for (const auto &comb : set_discrete_tails_combinations) {

    // ==================================================
    // First, convert the CG regression of Z to the form the same as the paper described,
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
      double coeff = Z.coefficients[comb].at(pos_of_y);
      Z.coefficients[comb].erase(Z.coefficients[comb].begin() + pos_of_y, Z.coefficients[comb].begin() + pos_of_y + 1);
      Z.coefficients[comb].push_back(coeff);
    }

    // ==================================================
    // For implementation, the ordering of other tail variables should be the same between Z and Y.
    // Here, I adjust Z to be the same ordering as Y.
    for (int i=0; i<Y.contin_tail_indexes.size(); ++i) {
      if (Y.contin_tail_indexes.at(i)!=Z.contin_tail_indexes.at(i)) {
        for (int j=i+1; j<Y.contin_tail_indexes.size(); ++j) {
          if (Z.contin_tail_indexes.at(j)==Y.contin_tail_indexes.at(i)) {
            // Swap.
            int temp_var = Z.contin_tail_indexes.at(i);
            Z.contin_tail_indexes.at(i) = Z.contin_tail_indexes.at(j);
            Z.contin_tail_indexes.at(j) = temp_var;
            int temp_coeff = Z.coefficients[comb].at(i);
            Z.coefficients[comb].at(i) = Z.coefficients[comb].at(j);
            Z.coefficients[comb].at(j) = temp_coeff;
            break;
          }
        }
      }
    }

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