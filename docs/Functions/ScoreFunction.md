# class ScoreFunction

## ScoreFunction(Network \*net, Trainer \*trn)

## double LogLikelihoodForNode(Node \*node_ptr)
For the i-th node, the log likelihood value is
```
(LaTeX)
$$
LL(i) = \sum_{j=1}^{q_i} \sum_{k=1}^{r_i} N_{ijk} log( \frac{N_{ijk}}{N_{ij}} )
$$
```

Input: a node pointer

Output: log likelihood of that node in the network


## double LogLikelihood()
```
(LaTeX)
$$ LL = \sum_{i=1}^{n} LL(i) $$
```

Output: log-likelihood of the network (sum of the log-likelihood of each node)

## double K2()
Output: **log** of the K2 score

The reason why using log is that it may need to calculate the factorial of a large number (e.g. 2000!) causing overflow. So, I need to turn to the log space.

The original equation is in the paper *[A Bayesian Method for the Induction of Probabilistic Networks from Data (Cooper, 1992)]*, which is
```
(LaTeX)
$$
P(B_S,D) = P(B_S)\prod_{i=1}^{n}\prod_{j=1}^{q_i}\frac{(r_i-1)!}{(N_{ij}+r_i-1)!}\prod_{k=1}^{r_i}N_{ijk}!
$$
```

So, the equation in the log space is
```
(LaTeX)
\begin{align}
log(P(B_S,D)) &= log(P(B_S))+ \sum_{i=1}^{n}\sum_{j=1}^{q_i}\bigg(log(\frac{(r_i-1)!}{(N_{ij}+r_i-1)!})+\sum_{k=1}^{r_i}log(N_{ijk}!)\bigg) \\
&= log(P(B_S))+ \sum_{i=1}^{n}\sum_{j=1}^{q_i}\bigg(\big(\sum_{a=r_i-1}^{1}log(a)\big) - \big(\sum_{b=N_{ij}+r_i-1}^{1}log(b)\big)+\sum_{k=1}^{r_i}\sum_{c=N_{ijk}}^{1}log(c)\bigg)
\end{align}
```

## double BDe()


## double BDeu(int equi_sample_size = 10)
Output: **log** of the BDeu score

The reason why using log is that it may need to calculate the factorial of a large number (e.g. 2000!) causing overflow. So, I need to turn to the log space.

The original equation is in *[Learning Bayesian Networks The Combination of Knowledge and Statistical Data (Heckerman, 1995)]*.
```
(LaTeX)
$$
p(D,B_s^h|\xi) = p(B_s^h|\xi)\cdot\prod_{i=1}^{n}\prod_{j=1}^{q_i}\frac{\Gamma(N_{ij}')}{\Gamma(N_{ij}'+N_{ij})}\prod_{k=1}^{r_i}\frac{\Gamma(N_{ijk}'+N_{ijk})}{\Gamma(N_{ijk}')}
$$
where $N_{ijk}' = N' p(x_i=k,\Pi_i=j|B_{sc}^h,\xi)$ and $N'$ is the equivalent sample size
```

After change it to the logarithm form, we can approximate the log of a Gamma function of a real number using the log of a factorial of an integer. That is,
```
(LaTeX)
$$ log(\Gamma(x)) \approx log((ceiling(x)-1)!) $$
```


## double AIC()
Output: the AIC score of the network

```
(LaTeX)
$$
MDL = LL - |B|
$$
where $|B|$ is the network complexity,
that is, the number of parameters
```

## double BIC()
Output: the BIC score of the network

The same as MDL.

## double MDL()
Output: the MDL score of the network


```
(LaTeX)
$$
MDL = LL - \frac{1}{2} log(N)|B|
$$
where $|B|$ is the network complexity,
that is, the number of parameters
```