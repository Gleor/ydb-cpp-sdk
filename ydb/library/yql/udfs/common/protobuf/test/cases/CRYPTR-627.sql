/* syntax version 1 */
/*
    Should check is TryParse work correct for bad protobuf input data
*/

$config = @@{"name":"yabs.proto.Profile","meta":"eNqNVt1y21QQRrJs2Ws7ceS0UVwKoQyDgWla0h+GXHTGScw0M00T0hQuNYp0bGuiP86RkvoxeAMegeE5uOcF+ga8AHtWUiynypQr+6y+/Tm73+4eeG/C/bl9Lh7FPEqiR6lg3MK/E89n2yQyQH7O/j/4ywT9JPtqbEPdS1ggTGWrNmzvfL69wG3nmOL3EHHGC1hz7IRNIz4vPAhTJd0vq3T3c/DCXyu2hYgjngizRnr3qvRepufk7yE0nSgNE8aFqd0O388wxmNoRJOJBNcJvFUFPpaIIqBvQJul58JsfDyWb0H/LWXcwxvrhN6sQv+MkLnxHejndhjKSJqEHVRh9whifAWaG8TCbBFyowp5EMTGDrT9wJowO0k5xgCEvl+FfhX8lKGMH2GFs2nq29xyooi7wmyT2hdVamenGXRfIo0D6Nup6yUR99LAEmwasBCL1tlSUP/rSv3RNf5NDjeeQ8eOY99D1nhRKMzu7XU5Gy2AA8zJie1xowv1icdFggRVh5qxAg3BnCh0kXR4HjwF7Yx70Udg8nMy87iLlJNae9D5lXnTWcLct4dhUqF9RZ9z7T6009hFJluJFzC0oQz1wenCxv8ItGSzVmVTI5t/KLB6s2Puwsp5YF03neeSg67Rg6YnOc9EFmbX+BTWC4m1HK861GV8YhZdyTaSYIzHwWRfyE6RZwOAXWLFMpUGqRigBbYXItuVYXNXS3jKBu8V6Cw1ECrmHVrEphnr0KE2RIkVuM8WabQdWVzr3KPuV27Jg7EBbcF85mA085hhhMqwu6vu/HAdUWMR0YM3sDIis3todRymAUbUOBifjQ5f9T4xdKiNDg56itGB5snb0/2Xozfjniohp+Oj41/GvRoaXTkd7x8fHY1fH4zODo9f97TBPyq0y3MPFS7Y/Ar7YlGBG6GrlDMMPcUqWJe2n7JsxGl7Kvp/CO0YeVJ8yIaZWTlvJZ0QniCxC3j9djjx/wWsX+VstMr+G7e321IHlPXLYeof16dwseIC4w2nmSIOPWXYuS5Xq0Sg3xXQi4mKJMThm2VUydqUuIS5VD5MMPWdFDqc3SDMcnmIL8Yd6ErjkkEZ4Rrko4rUI9CLDfIhnbvGKtTQOq25rJZrUM+uKcurSNHgCFqLqVtlpA96PrpLhjZu3rA21Mnc3wo08u2wBq1slSz6q5J5/eWmqVESMKOMLq/RUQ4T2ZOpn3g4axmnXKnYHBq3wwtKkSo9xjNuCyY96pQ1nDZy99H8aS7lsVRbvE7TE5YIbJ7gepLy+sT2BRv8W4N6thjLhpTcEGSShL3LKt+S8cyKGdEtjy7lxujKz2IeOsyl8GmUzaKUCyuwxUUe/3UemsXUKeeqRXr3oO9EvpRG+Hyyk1lGGyANzFyCKRK+zDrJ2yRHlsWcuZ4jO8dJOO3H7g1x7ODek2ITemnovVsazytVtF4lIaYBB//3Zo+Us9OOuVY6PTGN0ump2S+dnpnrpdNz806RrSC0bNc17+J5jYptczn5sSIbdKdNWEtsPmWJ5UayxjTDTfq0AatTJL7l+KnI6b1ZVFFg0h1KjjmQssEZ1OS7BT3i86YoeMXclFddh+b1E0O2QZf6oyCZVmrWGLrLDxWkFFbFS1KXUX/I/mz5UTjNRCqJKmcJ4vB1JDcSBqfl5Mk81ksej6Ff9bTpgebbtPCLdkYJVv2y1ODr0JL+RGIHceZ0V3k8+BOXaPm1I6PDHMu5PbPFbNEYDnee7GQytWpd1vLcdbwQXfh+eSYi12R4lty5lzm+Tl8+g7u5MIhCpPmEM+zA0JlTAylVI3JPPVH+AxssP0w=", "lists": {"optional": false}}@@;

$udf = Udf(Protobuf::TryParse, $config as TypeConfig);

SELECT $udf(ProfileDump) AS Profile
FROM Input;
