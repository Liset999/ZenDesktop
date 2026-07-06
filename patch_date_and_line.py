import re

filepath = r"d:\Repository\ZenDesktop_OneKeyDeploy\local@zen-taskbar-acrylic.wh.cpp"

with open(filepath, "r", encoding="utf-8") as f:
    content = f.read()

# Let's define the helper function to clean up a theme block
def clean_theme_block(theme_name, is_full_width, code_content):
    # Regex to capture the theme block
    pattern = r"(const Theme " + re.escape(theme_name) + r"\s*=\s*\{\{\n)(.*?)(\n\s*\}\s*,\s*\{)"
    
    match = re.search(pattern, code_content, re.DOTALL)
    if not match:
        print(f"Could not find theme block for {theme_name}")
        return code_content
    
    prefix = match.group(1)
    body = match.group(2)
    suffix = match.group(3)
    
    # Split the body into individual ThemeTargetStyles blocks
    # A block looks like: ThemeTargetStyles{L"...", { ... }}
    style_blocks = re.findall(r"(ThemeTargetStyles\{L\".*?\",\s*\{.*?\}\})", body, re.DOTALL)
    
    cleaned_styles = []
    for style in style_blocks:
        # Check if this block is for TimeInnerTextBlock or DateInnerTextBlock
        if 'TextBlock#TimeInnerTextBlock' in style or 'TextBlock#DateInnerTextBlock' in style:
            print(f"[{theme_name}] Removing style: {style.strip()}")
            continue
        
        # If this is full-width and is the RootGrid block, remove BorderThickness and BorderBrush
        if is_full_width and 'Taskbar.TaskbarFrame > Grid#RootGrid' in style:
            print(f"[{theme_name}] Modifying RootGrid style to remove border line")
            # Replace:
            # L"BorderThickness=$Apple_BorderThickness",
            # L"BorderBrush:=$Apple_BorderBrush",
            # L"Background:=$Apple_Background"
            # With:
            # L"BorderThickness=0",
            # L"Background:=$Apple_Background"
            modified_style = style
            modified_style = re.sub(r'L"BorderThickness=\$Apple_BorderThickness",?\s*', 'L"BorderThickness=0",\n        ', modified_style)
            modified_style = re.sub(r'L"BorderBrush:=\$Apple_BorderBrush",?\s*', '', modified_style)
            cleaned_styles.append(modified_style)
        else:
            cleaned_styles.append(style)
    
    # Join with proper formatting
    new_body = ",\n".join(cleaned_styles)
    # Ensure a trailing comma after the last item if it doesn't end with one (Theme structure style separator)
    if not new_body.rstrip().endswith(","):
        new_body += ","
        
    return code_content.replace(match.group(0), prefix + new_body + suffix)

patched = content
patched = clean_theme_block("g_themeAppleLiquidGlass", True, patched)
patched = clean_theme_block("g_themeAppleLiquidGlass_variant_Alternate", False, patched)
patched = clean_theme_block("g_themeAppleLiquidGlass_variant_Classic", True, patched)
patched = clean_theme_block("g_themeAppleLiquidGlass_variant_Classic_Alternate", False, patched)

with open(filepath, "w", encoding="utf-8") as f:
    f.write(patched)

print("Patching of date visibility and top taskbar line completed successfully!")
