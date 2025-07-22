reg add HKLM\SYSTEM\CurrentControlSet\Services\W32Time\TimeProviders\NtpClient /v SpecialPollInterval /t reg_dword /d 120 /f

# Defaults
$T0 = "0.pool.ntp.org tick.usno.navy.mil time-b.nist.govtime-b.nist.gov" 

# Kuiper
$T1 = "10.30.3.20 128.196.248.3"

# VATT
$T2 = "10.0.3.17 128.196.248.3"

# LBTO
$T3 = "128.196.248.150"

# Lemmon
$T4 = "128.196.248.3"

# Bok
$T5 = "10.30.3.7 140.252.86.113"

# UITS
$T6 = "140.252.86.113"

$SERVERS = -join ($T1, $T2, $T3, $T4, $T5, $T6, $T0)

Write-Host $SERVERS

w32tm /config /manualpeerlist:$SERVERS /syncfromflags:MANUAL /reliable:NO /update

net stop w32time
net start w32time
w32tm /resync /rediscover
