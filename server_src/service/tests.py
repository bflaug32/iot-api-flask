import unittest
import os
import sys

sys.path.append(os.path.join(os.path.dirname(__file__), ".."))
sys.path.append(os.path.join(os.path.dirname(__file__), '../env/lib/python3.4/site-packages'))


class UrlTest(unittest.TestCase):
    def setUp(self):
        from service import app
        self.app = app.test_client()

    def tearDown(self):
        pass

    def test_set_and_get_alarm(self):
        from service import app
        response = self.app.get('/api/v1/setalarm?h=2&m=10&key=%s' % app.config['API_KEY'])
        self.assertEqual(response.status_code, 200)
        print(response)
        self.assertTrue('SUCCESS' in str(response.data))
        response = self.app.get('/api/v1/getalarm')
        self.assertEqual(response.status_code, 200)


if __name__ == '__main__':
    unittest.main()
