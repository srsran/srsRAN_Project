#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

############################################
#            Pretty-Printers
############################################

python

###### static_vector<T, N> ########

class StaticVectorPrinter(object):
    def __init__(self, val):
        self.val = val
        self.value_type = self.val.type.template_argument(0)

    def children(self):
        start = self.val['buffer']['_M_elems'].cast(self.value_type.pointer())
        length = int(self.val['size_'])
        for idx in range(length):
            yield f'[{idx}]', start[idx]

    def to_string(self):
        length = int(self.val['size_'])
        capacity = int(self.val.type.template_argument(1))
        return f'static_vector of length {length}, capacity {capacity}'

    def display_hint(self):
        return 'array'


def make_static_vector(val):
    if 'static_vector<' in str(val.type):
        return StaticVectorPrinter(val)
gdb.pretty_printers.append(make_static_vector)


###### bounded_bitset<N, reversed> ########

class BoundedBitsetPrinter(object):
  def __init__(self, val):
      self.val = val
  
  def to_string(self):
      length = int(self.val['cur_size'])
      capacity = int(self.val.type.template_argument(0))
      buffer = self.val['buffer']
      bitstring = ''
      nof_words = (length + 63) // 64
      nof_bits_in_word = 64
      for idx in reversed(range(nof_words)):
          bitstring += '{:064b}'.format(int(buffer[idx]))
      # last word might have a lower number of bits
      last_word_nof_bits = length % 64
      bitstring = bitstring[64-last_word_nof_bits::] 
      return f'bounded_bitset of length {length}, capacity {capacity} = {bitstring}'
  
  def display_hint(self):
      return 'string'


def make_bounded_bitset(val):
    if 'bounded_bitset<' in str(val.type):
      return BoundedBitsetPrinter(val)
gdb.pretty_printers.append(make_bounded_bitset)


end

