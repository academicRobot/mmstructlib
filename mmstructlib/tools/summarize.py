
def from_atom(lst, attr, func=sum):
    for item in lst:
        setattr(item, attr, func([getattr(a, attr) for a in item.atoms]))
    
