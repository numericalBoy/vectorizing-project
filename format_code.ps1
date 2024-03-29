dir -r | % {if (
$_ -like "*.cpp" -or
$_ -like "*.c" -or
$_ -like "*.inl" -or
$_ -like "*.cu" -or
$_ -like "*.cc" -or
$_ -like "*.cxx" -or
$_ -like "*.h" -or
$_ -like "*.hxx" -or
$_ -like "*.hpp")
{
.\AStyle.exe --style=allman --indent=tab=4 --align-pointer=type --align-reference=type --max-code-length=80 --indent-preprocessor --break-blocks --pad-oper --pad-header --unpad-paren --delete-empty-lines --add-one-line-brackets --keep-one-line-blocks --keep-one-line-statements --convert-tabs --break-after-logical --mode=c $_.fullname
}
}