"""
core.project - project abstraction
"""

from core.project.new import new_project

__all__ = [
    'new'
]

#---
# Public
#---

def new(projpath):
    """ create a new project from default template """
    return new_project(projpath)
