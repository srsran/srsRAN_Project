#
# Copyright 2021-2025 Software Radio Systems Limited
#
# This file is part of srsRAN
#
# srsRAN is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsRAN is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

############################################
#            Pretty-Printers
############################################

# If you add a pretty printer, please create the corresponding test in
# tests/utils/gdb/pretty_printers

python

import struct

###### static_vector<T, N> ########

class StaticVectorPrinter(object):
    # Test: tests/utils/gdb/pretty_printers/pretty_printer_static_vector_test.cpp

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
    # Test: tests/utils/gdb/pretty_printers/pretty_printer_bounded_bitset_test.cpp

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
        return 'array'


def make_bounded_bitset(val):
    s = str(val.type.strip_typedefs())
    if 'bounded_bitset<' in s and s.endswith('>'):
        return BoundedBitsetPrinter(val)

gdb.pretty_printers.append(make_bounded_bitset)

###### tiny_optional<T> #######

class TinyOptionalPrinter(object):
    # Test: tests/utils/gdb/pretty_printers/pretty_printer_tiny_optional_test.cpp

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
        return 'array'

    @staticmethod
    def get_has_value(gdb_val):
        fields = gdb_val.type.strip_typedefs().fields()
        assert len(fields) > 0
        f_type_str = str(fields[0].type.strip_typedefs())
        if f_type_str.startswith('std::optional<'):
            return gdb_val['_M_payload']['_M_engaged']
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
        if f_type_str.startswith('std::optional<'):
            return gdb_val['_M_payload']['_M_payload']['_M_value']
        return gdb_val['val']


def make_tiny_optional(val):
    s = str(val.type.strip_typedefs())
    if s.startswith('srsran::tiny_optional<') and s.endswith('>'):
        return TinyOptionalPrinter(val)

gdb.pretty_printers.append(make_tiny_optional)

###### slotted_array<T, N> #######

class SlotArrayPrinter(object):
    # Test: tests/utils/gdb/pretty_printers/pretty_printer_slotted_array_test.cpp

    def __init__(self, val):
        self.val = val
        self.value_type = self.val.type.strip_typedefs().template_argument(0)
        self.capacity = int(self.val.type.strip_typedefs().template_argument(1))
        self.nof_elems = int(self.val['nof_elems'])

    def children(self):
        opt_value = gdb.lookup_type(f'srsran::tiny_optional<{self.value_type}>')
        vec_size = self.val['vec']['sz']
        vec = self.val['vec']['array']['_M_elems'].cast(opt_value.pointer())
        count = 0
        for idx in range(vec_size):
            if TinyOptionalPrinter.get_has_value(vec[idx]):
                yield f'{count}', f'{idx}'
                count += 1
                yield f'{count}', TinyOptionalPrinter.get_value(vec[idx])
                count += 1

    def to_string(self):
        return f'slotted_array of {self.nof_elems} elements, capacity {self.capacity}'

    def display_hint(self):
        return 'map'

def make_slotted_array(val):
    s = str(val.type.strip_typedefs())
    if s.startswith('srsran::slotted_array<') and s.endswith('>'):
        return SlotArrayPrinter(val)

gdb.pretty_printers.append(make_slotted_array)

###### slotted_vector<T> #######

class SlotVectorPrinter(object):
    # Test: tests/utils/gdb/pretty_printers/pretty_printer_slotted_vector_test.cpp

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
        count = 0
        for idx in range(nof_idxs):
            if int(indexmapper_ptr[idx]) != max_int:
                yield f'{count}', f'{idx}'
                count += 1
                yield f'{count}', object_ptr[indexmapper_ptr[idx]]
                count += 1

    def to_string(self):
        return f'slotted_vector of {self.nof_elems} elements'

    def display_hint(self):
        return 'map'

def make_slotted_vector(val):
    s = str(val.type.strip_typedefs())
    if s.startswith('srsran::slotted_vector<') and s.endswith('>'):
        return SlotVectorPrinter(val)

gdb.pretty_printers.append(make_slotted_vector)

###### Brain Floating Point 16 (bf16_t) ######

class BFloat16(object):
    # Test: tests/utils/gdb/pretty_printers/pretty_printer_bf16_test.cpp

    def __init__(self, val):
        self.__val = val

    def to_string(self):
        value_uint16 = self.__val['val']
        value_uint32 = value_uint16.cast(gdb.lookup_type('uint32_t')) << 16
        value_float = struct.unpack('!f', struct.pack('!I', value_uint32))[0]
        return value_float

    def display_hint(self):
        return None

def make_bf16_t(val):
    s = str(val.type.strip_typedefs())
    if 'srsran::strong_bf16_tag' in s:
        return BFloat16(val)

gdb.pretty_printers.append(make_bf16_t)

class BFloat16Complex(object):
    # Test: tests/utils/gdb/pretty_printers/pretty_printer_cbf16_test.cpp

    def __init__(self, val):
        self.__val = val

    def to_string(self):
        return f'{self.__val["real"]} + {self.__val["imag"]}i'

    def display_hint(self):
        return None

def make_cbf16_t(val):
    s = str(val.type.strip_typedefs())
    if s == 'srsran::cbf16_t':
        return BFloat16Complex(val)

gdb.pretty_printers.append(make_cbf16_t)

end

