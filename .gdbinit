#
# Copyright 2021-2024 Software Radio Systems Limited
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
        start = self.val['array']['_M_elems'].cast(self.value_type.pointer())
        length = int(self.val['sz'])
        for idx in range(length):
            yield f'[{idx}]', start[idx]

    def to_string(self):
        length = int(self.val['sz'])
        capacity = int(self.val.type.template_argument(1))
        return f'static_vector of length {length}, capacity {capacity}'

    def display_hint(self):
        return 'array'


def make_static_vector(val):
    s = str(val.type.strip_typedefs())
    if 'static_vector<' in s and s.endswith('>'):
        return StaticVectorPrinter(val)
gdb.pretty_printers.append(make_static_vector)


###### bounded_bitset<N, reversed> ########

class BoundedBitsetPrinter(object):
  def __init__(self, val):
      self.val = val
  
  def to_string(self):
      length = int(self.val['cur_size'])
      capacity = int(self.val.type.template_argument(0))
      buffer = self.val['buffer']['_M_elems']
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
    s = str(val.type.strip_typedefs())
    if 'bounded_bitset<' in s and s.endswith('>'):
        return BoundedBitsetPrinter(val)

gdb.pretty_printers.append(make_bounded_bitset)

###### optional<T> #######

class OptionalPrinter(object):
  def __init__(self, val):
      self.val = val
      self.value_type = self.val.type.strip_typedefs().template_argument(0)

  def children(self):
      has_val = bool(self.val['storage']['has_val'])
      if has_val:
          payload = self.val['storage']['payload']['val']
          yield '[0]', payload

  def to_string(self):
      has_val = bool(self.val['storage']['has_val'])
      if has_val:
          return 'optional (present)'
      return 'optional (empty)'

  def display_hint(self):
      return 'string'


def make_optional(val):
    s = str(val.type.strip_typedefs())
    if s.startswith('srsran::optional<') and s.endswith('>'):
        return OptionalPrinter(val)

gdb.pretty_printers.append(make_optional)


###### tiny_optional<T> #######

class TinyOptionalPrinter(object):
    def __init__(self, val):
        self.val = val
        self.has_val = TinyOptionalPrinter.get_has_value(self.val)

    def children(self):
        if self.has_val:
            yield '[0]', TinyOptionalPrinter.get_value(self.val)

    def to_string(self):
        if self.has_val:
            return 'tiny_optional (present)'
        return 'tiny_optional (empty)'

    def display_hint(self):
        return 'string'

    @staticmethod
    def get_has_value(gdb_val):
        fields = gdb_val.type.strip_typedefs().fields()
        assert len(fields) > 0
        f_type_str = str(fields[0].type.strip_typedefs())
        if f_type_str.startswith('srsran::optional<'):
            return bool(gdb_val['storage']['has_val'])
        if 'std::unique_ptr<' in str(gdb_val['val'].type):
            val_str = str(gdb_val['val'])
            val_str = val_str[val_str.find('get() = ') + len('get() = ')::]
            val_str = val_str[0:val_str.find('}')]
            val_int = int(val_str, 16)
            return val_int != 0
        return True #TODO: tiny_optional with flag

    @staticmethod
    def get_value(gdb_val):
        fields = gdb_val.type.strip_typedefs().fields()
        f_type_str = str(fields[0].type.strip_typedefs())
        if f_type_str.startswith('srsran::optional<'):
            return gdb_val['storage']['payload']['val']
        return gdb_val['val']


def make_tiny_optional(val):
    s = str(val.type.strip_typedefs())
    if s.startswith('srsran::tiny_optional<') and s.endswith('>'):
        return TinyOptionalPrinter(val)

gdb.pretty_printers.append(make_tiny_optional)

###### slotted_array<T, N> #######

class SlotArrayPrinter(object):
  def __init__(self, val):
      self.val = val
      self.value_type = self.val.type.strip_typedefs().template_argument(0)
      self.capacity = int(self.val.type.strip_typedefs().template_argument(1))
      self.nof_elems = int(self.val['nof_elems'])

  def children(self):
      vec = self.val['vec']['_M_elems']
      for idx in range(self.capacity):
          if TinyOptionalPrinter.get_has_value(vec[idx]):
              yield f'[{idx}]', TinyOptionalPrinter.get_value(vec[idx])

  def to_string(self):
      return f'slotted_array of {self.nof_elems} elements, capacity {self.capacity}'

  def display_hint(self):
      return 'string'

def make_slotted_array(val):
    s = str(val.type.strip_typedefs())
    if s.startswith('srsran::slotted_array<') and s.endswith('>'):
        return SlotArrayPrinter(val)

gdb.pretty_printers.append(make_slotted_array)

###### slotted_vector<T> #######

class SlotVectorPrinter(object):
    def __init__(self, val):
        self.val = val
        self.value_type = self.val.type.strip_typedefs().template_argument(0)
        self.objects = self.val['objects']
        self.nof_elems = int(self.objects['_M_impl']['_M_finish'] - self.objects['_M_impl']['_M_start'])

    def children(self):
        indexmapper = self.val['index_mapper']
        nof_idxs = int(indexmapper['_M_impl']['_M_finish'] - indexmapper['_M_impl']['_M_start'])
        max_int = 2**64 - 1
        indexmapper_ptr = indexmapper['_M_impl']['_M_start']
        object_ptr = self.objects['_M_impl']['_M_start']
        for idx in range(nof_idxs):
            if int(indexmapper_ptr[idx]) != max_int:
                yield f'[{idx}]', object_ptr[indexmapper_ptr[idx]]

    def to_string(self):
        return f'slotted_vector of {self.nof_elems} elements'

    def display_hint(self):
        return 'string'

def make_slotted_vector(val):
    s = str(val.type.strip_typedefs())
    if s.startswith('srsran::slotted_vector<') and s.endswith('>'):
        return SlotVectorPrinter(val)

gdb.pretty_printers.append(make_slotted_vector)

end

