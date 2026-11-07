"""Compatibility entrypoint for widget integration validation."""
import runpy
from pathlib import Path
runpy.run_path(str(Path(__file__).with_name('check_widgets_integration.py')),run_name='__main__')
