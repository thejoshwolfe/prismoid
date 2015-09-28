def value_matcher(value):
  if value.type.name in ("String", "Reference<StringImpl>"): return StringPrinter(value)
  if value.type.tag == "StringImpl": return StringImplPrinter(value)
  return None

class StringPrinter(object):
  def __init__(self, value):
    self.value = value
  def display_hint(self):
    return "string"
  def to_string(self):
    target_pointer = self.value["_target"]
    if not target_pointer:
      return "<NULL>" # ambiguous, but whatever
    stringimpl_value = target_pointer.dereference()
    return StringImplPrinter(stringimpl_value).to_string()

class StringImplPrinter(object):
  def __init__(self, value):
    self.value = value
  def display_hint(self):
    return "string"
  def to_string(self):
    chars_list = self.value["_chars"]
    length_value = chars_list["_length"]
    items_value = chars_list["_items"]
    result = ""
    i = 0
    while i < length_value:
      char = (items_value + i).dereference()
      result += chr(char) # possibly O(n^2) depending on the python implementation
      i += 1
    return result

gdb.current_objfile().pretty_printers.append(value_matcher)
