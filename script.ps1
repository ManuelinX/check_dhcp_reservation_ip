

$scopes = @(
    "10.178.0.0",
    "10.178.1.0",
    "10.178.2.0",
    "10.178.3.0",
    "10.178.4.0",
    "10.178.5.0",
    "10.178.6.0",
    "10.178.7.0",
    "10.178.8.0",
    "10.178.9.0",
    "10.178.10.0",
    "10.178.11.0",
    "10.178.12.0",
    "192.168.118.0",
    "192.168.120.0"
)

$result = foreach ($scope in $scopes) {
    Get-DhcpServerv4Reservation -ScopeId $scope |
    Select-Object @{N='Scope';E={$scope}}, IPAddress, ClientId, Name, Description
}

$result | Export-Csv "reservaciones_dhcp.csv" -NoTypeInformation -Encoding UTF8