from yaml import safe_load
from uber_rides.client import UberRidesClient
from uber_rides.session import Session


def findRides(lat=52.386, lon=4.873, minCapacity=-1, maxCapacity=-1, maxPrice=-1, maxTime=30, shared=False):
    print('uber?')
    session = Session(server_token='LfFtq8jnN1YYN0ZTeGetOvadi_DiCAk8nEForlLq')
    api_client = UberRidesClient(session)
    response = api_client.get_products(lat, lon)
    products = response.json.get('products')

    def not_shared(p):
        if p["shared"] and not shared:
            return False
        else:
            return p
    filteredProducts = [p for p in filter(not_shared, products)]

    if minCapacity > 0:
        filteredProducts = [p for p in filteredProducts if p['capacity'] >= minCapacity]

    if maxCapacity > 0:
        filteredProducts = [p for p in filteredProducts if p['capacity'] <= maxCapacity]

    if maxPrice > 0:
        filteredProducts = [p for p in filteredProducts if p['price_details']['minimum'] <= maxPrice]

    for p in filteredProducts:
        response = api_client.get_pickup_time_estimates(lat, lon, p['product_id'])
        time = response.json.get('times')[0]['estimate']
        p['pickup_time'] = time / float(60)

    filteredProducts = [p for p in filteredProducts if p['pickup_time'] <= maxTime]

    return filteredProducts