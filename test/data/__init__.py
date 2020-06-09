import os
path, _ = os.path.split(__file__)

def get_file_path(filename):
    """
    Returns path of given file.  Convience method for tests
    """
    return os.path.join(path, filename)

def get_file_contents(filename):
    """
    Returns contents of given file.  Convience method for tests
    """
    with open(get_file_path(filename), 'rb') as fh:
        contents = fh.read()
    return contents

