%% scan rampe (triangle)

function [i_scan, j_scan] = creer_scan(filename)

  data = loadpgm(filename) ;

  [idim, jdim] = size(data) ;

  [i_scan, j_scan] = creer_scan_avec_dim(idim, jdim) ;
   
end%fcn
