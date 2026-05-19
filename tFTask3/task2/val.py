# Sam Lunev. 2026. All Rights Reserved.
import math

def verify_results(filename='res.txt'):
    """
    Verifier for thread pool results with tolerance for output precision.
    """
    errors = []
    line_count = 0
    
    with open(filename, 'r') as f:
        for line_num, line in enumerate(f, 1):
            line = line.strip()
            if not line:
                continue
            
            line_count += 1
            parts = line.split()
            
            if len(parts) < 3:
                errors.append(f"Line {line_num}: Invalid format")
                continue
            
            operation = parts[0]
            
            try:
                if operation == "Sin":
                    if len(parts) != 4:
                        errors.append(f"Line {line_num}: Sin requires 3 values")
                        continue
                    
                    param = float(parts[1])
                    result = float(parts[2])
                    index = int(parts[3])
                    
                    expected = math.sin(param)
                    # Use larger tolerance for output precision differences
                    if not math.isclose(result, expected, rel_tol=1e-5):
                        errors.append(f"Line {line_num}: Sin({param}) = {result}, expected {expected}")
                    
                elif operation == "Sqrt":
                    if len(parts) != 4:
                        errors.append(f"Line {line_num}: Sqrt requires 3 values")
                        continue
                    
                    param = float(parts[1])
                    result = float(parts[2])
                    index = int(parts[3])
                    
                    if param < 0:
                        errors.append(f"Line {line_num}: Cannot compute sqrt of negative number {param}")
                        continue
                        
                    expected = math.sqrt(param)
                    if not math.isclose(result, expected, rel_tol=1e-5):
                        errors.append(f"Line {line_num}: Sqrt({param}) = {result}, expected {expected}")
                    
                elif operation == "Pow":
                    if len(parts) != 5:
                        errors.append(f"Line {line_num}: Pow requires 4 values")
                        continue
                    
                    base = float(parts[1])
                    exp = float(parts[2])
                    result = float(parts[3])
                    index = int(parts[4])
                      
                    expected = math.pow(base, exp)
                    if not math.isclose(result, expected, rel_tol=1e-5):
                        errors.append(f"Line {line_num}: Pow({base}, {exp}) = {result}, expected {expected}")
                              
            except (ValueError, IndexError) as e:
                errors.append(f"Line {line_num}: Parsing error - {str(e)}")
    
    print(f"Processed {line_count} lines")
    print(f"Found {len(errors)} errors:")
    for error in errors[:10]:  # Show first 10 errors
        print(f"  {error}")
    if len(errors) > 10:
        print(f"  ... and {len(errors) - 10} more errors")

if __name__ == "__main__":
    verify_results()