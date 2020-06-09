"""
"""

import os
import pickle
from operator import itemgetter
from itertools import groupby
import pkg_resources

#path, _ = os.path.split(__file__)
#path = os.path.join(path, "data")
default_file = "reduced_atom.siz"
default_pickle = "reduced_atom.pickle"

def load_default():
    #return load_siz_file(os.path.join(path, default_file))
    return load_siz_file(
        os.path.join(
            pkg_resources.get_default_cache(),
            pkg_resources.resource_filename(
                "mmstructlib.radii.data",
                default_file
            )
        )
    )

    #can't figure out how to load pickle :(
    #fh = pkg_resources.resource_stream("mmstructlib.radii", "data/"+default_pickle)
    #return pickle.load(fh)

def create_default(path):
    radii_set = load_siz_file(os.path.join(path, default_file))
    fh = open(os.path.join(path, default_pickle), "wb")
    pickle.dump(radii_set, fh)

class RadiiSet(object):
    #__module__ = "mmstructlib.radii.load_radii" #hack to get pickle to load
    #__module__ = "load_radii" #hack to get pickle to load
    def __init__(self, def_items):
        #print "module", RadiiSet.__module__
        self.default = 0.0
        self.monomer_radii = dict()
        self.aa_common_radii = dict()
        self.element_radii = dict()
        for monomer, radii_lst in groupby(sorted(def_items), itemgetter(0)):
            self.monomer_radii[monomer] = dict(
                [(a, float(r)) for _, a, r in radii_lst]
            )
        if '*' in self.monomer_radii:
            self.element_radii = self.monomer_radii['*']
            del self.monomer_radii['*']
        if '<AA>' in self.monomer_radii:
            for k, v in  self.monomer_radii['<AA>'].items():
                self.aa_common_radii[k] = v
            del self.monomer_radii['<AA>']

def load_siz_file(filename):
    import re
    fh = open(filename)
    parse_def = re.compile("\s*([^\s:]+)\s*:\s*([^\s:]+)\s+([\d\.]+)\s*")
    def_items = [parse_def.match(l.strip()).groups() for l in fh]
    return RadiiSet(def_items)

if __name__ == "__main__":
    path, _ = os.path.split(__file__)
    path = os.path.join(path, "data")
    create_default(path)
