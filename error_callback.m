function m = error_callback(e,f)

if isequal(f, 'SL_SFcnErrorStatus')
  m = sprintf('%s\nSee the command window for details', lasterr);
else
  m = lasterr;
end

