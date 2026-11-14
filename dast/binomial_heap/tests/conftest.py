import sys
from pathlib import Path

# make `import binomial_heap` work when pytest is run from any directory
sys.path.insert(0, str(Path(__file__).resolve().parent.parent))
