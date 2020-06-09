import argparse
import sys
import re
from operator import itemgetter
from mmstructlib.IO import load_cif_from_file, load_cif_from_mirror
from mmstructlib.tools import preprocess

STRIP_DASH = re.compile("^\-\-?")

class ArgParse(object):
    def __init__(self, description, mirror_default_path=None, *args, **kwargs):
        super(ArgParse, self).__init__(*args, **kwargs)
        self.parser = argparse.ArgumentParser(description=description)
        self._mirror_default_path = mirror_default_path
        self._has_mirror = False
        self._post = []

    def __call__(self, argv=sys.argv[1:]):
        args = self.parser.parse_args(argv)
        for post_func, post_args in self._post:
            post_func(args, *post_args)
        return args

    def error(self, msg):
        self.parser.error(msg)

    def _set_argument(self, std_args, std_kwargs, new_args, new_kwargs):
        kwargs = dict(std_kwargs)
        kwargs.update(new_kwargs)
        args = new_args if new_args else std_args
        if 'name' in kwargs:
            args = [kwargs['name']]
            del kwargs['name']
        self.parser.add_argument(*args, **kwargs)
        return max([ (len(x), STRIP_DASH.sub('', x)) for x in args ])[1]

    def add_argument(self, *args, **kwargs):
        self.parser.add_argument(*args, **kwargs)

    def struct_arg(self, *args, **kwargs):
        std_args = ['-m', '--mirror']
        std_kwargs = {
            'help' : 'interpret struct as an id, fetch from mirror directory'
        }
        if not self._has_mirror:
            if self._mirror_default_path is None:
                std_kwargs['nargs'] = 1
                #self.parser.add_argument(std_args, nargs=1, help=m_help)
            else:
                self._has_mirror = True
                std_kwargs['nargs'] = '?'
                std_kwargs['const'] = self._mirror_default_path
            self.parser.add_argument(*std_args, **std_kwargs)

        strip_water = kwargs.get('strip_water', False)
        if 'strip_water' in kwargs:
            del kwargs['strip_water']

        strip_hydrogen = kwargs.get('strip_hydrogen', False)
        if 'strip_hydrogen' in kwargs:
            del kwargs['strip_hydrogen']

        std_args = ['struct']
        std_kwargs = {
            'help' : 'cif structure file or (if --mirror) id'
        }
        name = self._set_argument(std_args, std_kwargs, args, kwargs)
        self._post.append([self.load_struct, [name, 'mirror', strip_water, strip_hydrogen]])

    def report_chain_arg(self, *args, **kwargs):
        std_args = ['-c', '--chain']
        std_kwargs = {
        #    'nargs' : 1,
            'action' : 'append',
            'metavar' : 'C',
            'help' : 'select chain for reporting, can be given multiple times'
        }

        struct = kwargs.get('struct', 'struct')
        if 'struct' in kwargs:
            del kwargs['struct']

        auto_fill = kwargs.get('auto_fill', True)
        if 'auto_fill' in kwargs:
            del kwargs['auto_fill']

        protein_only = kwargs.get('protein_only', True)
        if 'protein_only' in kwargs:
            del kwargs['protein_only']

        name = self._set_argument(std_args, std_kwargs, args, kwargs)
        self._post.append([self.report_chain, [name, struct, auto_fill, protein_only]])

    def outfile_arg(self, *args, **kwargs):
        std_args = ['outfile']
        std_kwargs = {
            'type' : lambda x: open(x, "w"),
            'nargs' : '?',
            'default' : sys.stdout,
            'help' : 'output file [default: stdout]'
        }
        name = self._set_argument(std_args, std_kwargs, args, kwargs)
    def load_struct(self, args, struct_var, mirror_var, strip_water, strip_hydrogen):
        struct_id = getattr(args, struct_var)
        if getattr(args, mirror_var) is not None:
            mirror_path = getattr(args, mirror_var)
            struct = load_cif_from_mirror(struct_id, mirror_path)
        else:
            struct = load_cif_from_file(struct_id)
        if strip_water:
            for model in struct:
                preprocess.strip_water(model)
        if strip_hydrogen:
            for model in struct:
                preprocess.strip_hydrogen(model)
        setattr(args, struct_var, struct)

    def report_chain(self, args, chain_var, struct_var, auto_fill, protein_only):
        chains = getattr(args, chain_var)
        struct = getattr(args, struct_var)
        if protein_only:
            candidate_chains = struct[0].proteins
        else:
            candidate_chains = struct[0].molecules
        if chains is None:
            if auto_fill:
                chains = candidate_chains
        else:
            if isinstance(chains, str):
                chains = [chains]
            #following errors out for some pdb files that use same id for
            #   multiple molecules
            #chains = [ (chain, struct[0].polymer_by_pdb_id(chain)) for chain in chains ]
            all_chains = [(c_obj.pdb_id, c_obj) for c_obj in candidate_chains ]
            #chains = filter(lambda x: x[0] in chains, all_chains)
            chains = [x for x in all_chains if x[0] in chains]
            errorchains = [ x[0] for x in chains if x[1] is None ]
            if errorchains:
                self.parser.error('could not find specified chains: {0}'.format(','.join(errorchains)))
            chains =  list(map(itemgetter(1),chains))
        setattr(args, chain_var, chains)


