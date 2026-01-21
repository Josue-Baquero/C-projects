function out = reconstruction_scan(vec_illu, idim, jdim)


  out = zeros(idim, jdim) ;
  [i_scan, j_scan] = creer_scan_avec_dim(idim, jdim) ;

  len = length(i_scan) ;
  if (len ~= length(vec_illu))
    disp('ERR: erreur de dimension') ;
    return
  end

  for n = 1:len

    out(floor(i_scan(n)) , floor(j_scan(n))) = vec_illu(n) ;

  end%for

end%fcn
