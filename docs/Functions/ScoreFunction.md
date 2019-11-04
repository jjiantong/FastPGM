# class ScoreFunction

## ScoreFunction(Network \*net, Dataset \*dts)

## ScoreForNode(Node \*node_ptr, string metric)

As the name suggests.

The argument metric can be one of the following

* log likelihood
* log K2
* log BDeu
* aic
* bic
* mdl

## double LogLikelihoodForNode(Node \*node_ptr)

For the i-th node, the log likelihood value is

```LaTeX
(LaTeX)
$$
LL(i) = \sum_{j=1}^{q_i} \sum_{k=1}^{r_i} N_{ijk} log( \frac{N_{ijk}}{N_{ij}} )
$$
```

Time complexity: O( number of configuration of parents times number of potential values of this node times number of instances of the dataset )

Input: a node pointer

Output: log likelihood of that node in the network


## double LogLikelihood()

```LaTeX
(LaTeX)
$$ LL = \sum_{i=1}^{n} LL(i) $$
```

Time complexity: O( number of nodes times complexity of each node )

Output: log-likelihood of the network (sum of the log-likelihood of each node)

## LogK2ForNode(Node \*node_ptr)

As the name suggests.

Time complexity: O( number of configuration of parents times number of potential values of this node times number of instances of the dataset )

## double LogK2()

Output: log of the K2 score

The equation in the log space is

```LaTeX
(LaTeX)
\begin{align}
log(P(B_S,D)) &= log(P(B_S))+ \sum_{i=1}^{n}\sum_{j=1}^{q_i}\bigg(log(\frac{(r_i-1)!}{(N_{ij}+r_i-1)!})+\sum_{k=1}^{r_i}log(N_{ijk}!)\bigg) \\
&= log(P(B_S))+ \sum_{i=1}^{n}\sum_{j=1}^{q_i}\bigg(\big(\sum_{a=r_i-1}^{1}log(a)\big) - \big(\sum_{b=N_{ij}+r_i-1}^{1}log(b)\big)+\sum_{k=1}^{r_i}\sum_{c=N_{ijk}}^{1}log(c)\bigg)
\end{align}
```

Time complexity: O( number of nodes times complexity of each node )

## double K2()

Prohibited.

The reason is that it may need to calculate the factorial of a large number (e.g. 2000!) causing overflow. So, I need to turn to the log space.

The original equation is in the paper *[A Bayesian Method for the Induction of Probabilistic Networks from Data (Cooper, 1992)]*, which is

```LaTeX
(LaTeX)
$$
P(B_S,D) = P(B_S)\prod_{i=1}^{n}\prod_{j=1}^{q_i}\frac{(r_i-1)!}{(N_{ij}+r_i-1)!}\prod_{k=1}^{r_i}N_{ijk}!
$$
```

## LogBDeuForNode(Node \*node_ptr, int equi_sample_size)

As the name suggests.

Time complexity: O( number of configuration of parents times number of potential values of this node times number of instances of the dataset )

## double LogBDeu(int equi_sample_size = 10)

Output: log of the BDeu score

After change it to the logarithm form, we can approximate the log of a Gamma function of a real number using the log of a factorial of an integer. That is,

```LaTeX
(LaTeX)
$$ log(\Gamma(x)) \approx log((ceiling(x)-1)!) $$
```

## BDeu(int equi_sample_size)

Prohibited. 

The reason is that it may need to calculate the factorial of a large number (e.g. 2000!) causing overflow. So, I need to turn to the log space.

The original equation is in *[Learning Bayesian Networks The DiscreteConfig of Knowledge and Statistical Data (Heckerman, 1995)]*.

```LaTeX
(LaTeX)
$$
p(D,B_s^h|\xi) = p(B_s^h|\xi)\cdot\prod_{i=1}^{n}\prod_{j=1}^{q_i}\frac{\Gamma(N_{ij}')}{\Gamma(N_{ij}'+N_{ij})}\prod_{k=1}^{r_i}\frac{\Gamma(N_{ijk}'+N_{ijk})}{\Gamma(N_{ijk}')}
$$
where $N_{ijk}' = N' p(x_i=k,\Pi_i=j|B_{sc}^h,\xi)$ and $N'$ is the equivalent sample size
```

## AICForNode(Node \*node)

As the name suggests.

Time complexity: the same as `LogLikelihoodForNode` because of the definition

## double AIC()

Output: the AIC score of the network

```LaTeX
(LaTeX)
$$
MDL = LL - |B|
$$
where $|B|$ is the network complexity,
that is, the number of parameters
```

## BICForNode(Node \*node)

As the name suggests.

Time complexity: the same as `LogLikelihoodForNode` because of the definition

## double BIC()

Output: the BIC score of the network

The same as MDL.

## MDLForNode(Node \*node)

As the name suggests.

Time complexity: the same as `LogLikelihoodForNode` because of the definition

## double MDL()

Output: the MDL score of the network

```LaTeX
(LaTeX)
$$
MDL = LL - \frac{1}{2} log(N)|B|
$$
where $|B|$ is the network complexity,
that is, the number of parameters
```

## PrintAllScore()

Print all score of the network on the dataset.

LogLikelihood, AIC, BIC, MDL, LogK2, LogBDeu.