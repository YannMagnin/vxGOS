"""
core.project.new - create a new project from default template
"""
import os
import shutil

from core.logger import log

__all__ = [
    'new_project'
]

#---
# Public
#---

# (todo/CDE6) : change internal project name
def new_project(project_path):
    """ create a new project """
    if os.path.exists(project_path):
        log.warn(f"The path {project_path} already exists !")
        return True
    origin_path = os.path.dirname(__file__)
    shutil.copytree(
        origin_path + '/../../assets/project/',
        project_path
    )
    log.user(f"project '{project_path}' successfully created !")
    return False
