import os
import importlib

# Get the current package name
package_name = __name__

# Loop through all files in the directory
for filename in os.listdir(os.path.dirname(__file__)):
    if filename.endswith(".py") and filename != "__init__.py":
        module_name = filename[:-3]  # Remove the ".py" extension

        # Import the module and retrieve all its attributes
        module = importlib.import_module(f".{module_name}", package_name)

        # Import everything (*) into the current namespace
        for attr_name in dir(module):
            if not attr_name.startswith("_"):  # Ignore private/internal attributes
                globals()[attr_name] = getattr(module, attr_name)
