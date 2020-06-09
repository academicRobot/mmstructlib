from mmstructlib.IO.cif import parse_cif, build
import os
import gzip

PDB_MIRROR_ENVVAR = "PDB_MIRROR"

def load_cif_from_file(filename):
    return build(parse_cif(open(filename, 'rb')))

def load_cif_from_mirror(pdb_id, mirror_path=None, allow_obsolete=True):
    if mirror_path is None:
        if PDB_MIRROR_ENVVAR in os.environ:
            mirror_path = os.environ[PDB_MIRROR_ENVVAR]
        else:
            raise ValueError("No mirror directory provided and environmental variable '{}' not defined".format(PDB_MIRROR_ENVVAR))
    file_path = os.path.join(mirror_path, 'data', 'structures', 'divided', 'mmCIF', pdb_id[1:3].lower(), pdb_id.lower() + '.cif.gz')
    if not os.path.isfile(file_path) and allow_obsolete:
        file_path = os.path.join(mirror_path, 'data', 'structures', 'obsolete', 'mmCIF', pdb_id[1:3].lower(), pdb_id.lower() + '.cif.gz')
    if not os.path.isfile(file_path):
        raise IOError("No file found for structure {0}".format(pdb_id))
    return build(parse_cif(gzip.open(file_path)))

