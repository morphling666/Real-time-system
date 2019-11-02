function init_kernel(cb,mask_values)

nip=0;
nop=0;

in_out_vec=mask_values{3};
block = [cb '/A//D'];
oldblocktype = get_param(block, 'BlockType');
if in_out_vec(1) == 0 
  newblocktype = 'Ground';
else
  newblocktype = 'Inport';
end
if ~isequal(newblocktype,oldblocktype)
  position = get_param(block, 'Position');
  orientation = get_param(block, 'Orientation');
  delete_block(block)
  add_block(['built-in/' newblocktype],block,'Position',position,'Orientation',orientation);
end
% output port changed?
block = [cb '/D//A'];
oldblocktype = get_param(block, 'BlockType');
if in_out_vec(2) == 0
  newblocktype = 'Terminator';
else
  newblocktype = 'Outport';
end
if ~isequal(newblocktype,oldblocktype)
  position = get_param(block, 'Position');
  orientation = get_param(block, 'Orientation');
  delete_block(block)
  add_block(['built-in/' newblocktype],block,'Position',position,'Orientation',orientation);
end
% Set correct input and output port numbers
if in_out_vec(1) > 0 
  nip = nip + 1;
  set_param([cb '/A//D'],'Port',num2str(nip))
end
if in_out_vec(2) > 0 
  nop = nop + 1;
  set_param([cb '/D//A'],'Port',num2str(nop))
end
end


