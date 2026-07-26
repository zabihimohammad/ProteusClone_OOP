import os

# آدرس خروجی را به صورت مستقیم روی دسکتاپ بگذارید
output_filename = r"C:\Users\Noor-Rayaneh\Desktop\all_project_code.txt"
# فرمت فایل‌هایی که می‌خواهیم جمع‌آوری کنیم
valid_extensions = (".cpp", ".h")
# پوشه‌ای که کدهای شما در آن قرار دارد
source_folder = "src"

with open(output_filename, "w", encoding="utf-8") as outfile:
    for root, dirs, files in os.walk(source_folder):
        for file in files:
            if file.endswith(valid_extensions):
                filepath = os.path.join(root, file)
                
                # ایجاد یک جداکننده زیبا برای مشخص شدن نام هر فایل
                outfile.write(f"\n{'='*60}\n")
                outfile.write(f"FILE: {filepath}\n")
                outfile.write(f"{'='*60}\n\n")
                
                # خواندن محتوای فایل و نوشتن آن
                try:
                    with open(filepath, "r", encoding="utf-8") as infile:
                        outfile.write(infile.read())
                        outfile.write("\n")
                except Exception as e:
                    outfile.write(f"Error reading file: {e}\n")

print(f"Done! All codes are saved in '{output_filename}'")