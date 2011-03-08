function runBugRank(bugRankArgsFile,outputFile)
% Function to run bugRank analysis
%
% ARGUMENTS:
% bugRankArgsFile -- fully qualified path name for .mat file which holds the
% arguments to feed into bugRank. 
%     Here is a summary of the various arguments
%     * predScores : A row vector of scores of bug predictors.
%     * E : The adjacency graph representation of the dependencies. E(i,j) =
%     1 if and only if node j is control (data) dependent on node
%     i. Alternatively it can be representative of dependencies over
%     groupings of Program Dependency Graph (PDG) nodes eg., we can say line
%     i is 'dependent' on line j if there is a node on line i which is
%     dependent on a node on line j. Right now the dependencies are only
%     being factored in as 0 or 1 i.e., a dependency either exists or it
%     doesn't. Further variations based on 'strengths' of such dependencies
%     can also be formulated.
%     * X : The bug predicates for which we have scores don't correspond 1-1
%     to the abstraction regd. which we have dependency information. This
%     matrix is a mapping from the scored predicates to the 'nodes' being
%     represented in E (whether they be individual nodes or grouped by
%     lines). It is a 0-1 matrix of dimension m by n where m is the number of
%     nodes in the underlying graph corresponding to E and n are the number
%     of predicates. If the number of predicates is the same as number of
%     nodes it defaults to the identity matrix.
%     * alpha : Is the probability of following links of the dependency graph
%     during the random walk. This also affects how much the score of a far
%     away node affects your own score (which is why we don't want it to be
%     high, or every node will depend on ever other). Defaults to 0.1, but
%     should depend on size of graph. TODO: set this automatically based on
%     size of graph i.e., larger graph warrants a higher value of alpha.
%     * beta : Is the probability mass spent exploring nodes about which we
%     have no associated score (either directly or indirectly due to mapping
%     matrix X). Defaults to 0.8 which is already pretty high, but is
%     somewhat data dependent. TODO: set this autotmatically based on
%     distribution of scores. A sparse widely spread distribution of scores
%     warrants a higher beta.
%
% outputFile -- Name of .mat file to save the resulting scores. The final
% scores are the elementwise harmonic mean combination of the result of
% performing two random walks: one following the dependency edges int he
% forward direction and the other in the reverse direction.

  load(bugRankArgsFile);

  % Required arguments
  if ~exist('predScores', 'var') | ~exist('E', 'var')
    fprint('Error: Missing arguments!\n');
    exit;
  end

  predScores = predScores(:)'; % ensure row vector
  isMappingOneOne = (size(predScores,2) == size(E,1));

  % DEFAULTS
  if size(E,1) ~= size(E,2)
    fprintf('Error: The adjacency matrix E is not square\n');
    exit;
  end

  if ~exist('X', 'var')
    if ~isMappingOneOne
      fprintf(['Error: Need to provide mapping from predicates in predScores' ...
	       ' to nodes in E. Cannot infer mapping matrix X, so exiting']);
      exit;
    end
  end

  if ~exist('beta', 'var')
    beta = 0.8;
  end
  if ~exist('alpha', 'var')
    alpha = 0.1;
  end


  if isMappingOneOne
    PS = predScores;
    NS = predScores';
  else
    PS = predScores ./ sum(X, 1);
    assert(max(sum(X,1))==1,['Error: There is more than one predicate associated' ...
		    ' with a node']);
    NS = sum(X * diag(PS), 2);
  end

  Z = getZmatrix(E',NS,alpha,beta);
  [resultant1, D, flag] = eigs(Z, 1, 'LM'); % doing forward walk
  assert(flag == 0, 'Error: EV calculation did not converge!');
  Z = getZmatrix(E,NS,alpha,beta);
  [resultant2, D, flag] = eigs(Z, 1, 'LM'); % doing backward walk
  assert(flag == 0, 'Error: EV calculation did not converge!');
  resultant = (resultant1 .* resultant2) ./ (resultant1 + resultant2);
  resultant(isnan(resultant)) = 0;
  resultant = resultant / sum(resultant); % return normalized result
  

  if isMappingOneOne
    result = resultant;
  else
    distributor = (X * diag(PS)) ./ (diag(NS) * X);
    distributor(isnan(distributor)) = 0;
    result = sum((diag(resultant) * X) .* distributor, 1);
  end

  save(outputFile,'result');

end
