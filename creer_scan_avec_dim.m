%% scan rampe (triangle)

function [i_scan, j_scan] = creer_scan_avec_dim(idim, jdim)

  idim = floor(idim/2)*2 ;
  jdim = floor(jdim/2)*2 ;

  scan = [ (1:1:jdim) ,  ((jdim-1) : -1 : 2)]  ;
  j_scan = [] ;
  
  for n = 1 : 1 : floor(idim/2)
    j_scan = [ j_scan , scan] ;
  end

  pas = 1/(idim*(jdim-1)) ;
  i_scan = 0 : pas : 1-pas ;
  i_scan = i_scan*idim + 1 ;
   
end%fcn
