def value_matcher(value):
  if value.type.tag == "rat64": return Rat64Printer(value)
  return None

class Rat64Printer(object):
  def __init__(self, value):
    self.value = value
  def to_string(self):
    numerator = self.value["numerator"]
    denominator = self.value["denominator"]
    return str(numerator) + "/" + str(denominator)

gdb.current_objfile().pretty_printers.append(value_matcher)
